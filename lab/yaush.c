#include "header.h"
#include "marcos.h"
#include "utils.h"
#include "jobs.h"
#include "handle.h"
#include "internal_command.h"

char last_dir[LAST_DIR_SIZE];
Job *jobs; //[MAX_JOBS];
int *job_count;

struct sigaction sa;

void set_prompt(char *prompt)
{
    // prompt shape: pwp@host cwd [time]
    struct passwd *pwp;
    char host[HOST_NAME_SIZE];
    char cwd[WORKING_DIR_SIZE];
    char time_str[TIME_SIZE];

    // get the pwpid
    pwp = getpwuid(getuid());

    // get the host name
    gethostname(host, HOST_NAME_SIZE);

    // get the current working directory
    getcwd(cwd, WORKING_DIR_SIZE);

    // get the time
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

    // format the prompt
    sprintf(prompt, "[\033[1;37m%s@%s\033[0m\033[1;36m %s\033[0m] \033[46;37m(%s)\033[0m\n ➜  ", pwp->pw_name, host, cwd, time_str);
}

void build_history_list()
{
    using_history();
    stifle_history(HISTORY_SIZE);
    read_history(HISTORY_FILE);
}

// Function to execute a single command with redirection and background support
void execute_command(char *cmd, int background)
{
    char *args[MAX_ARGS];
    int arg_count;
    char *input_file = NULL;
    char *output_file = NULL;
    int append_mode = 0;

    // copy the command to history
    char cmd_copy[COMMAND_SIZE];
    strncpy(cmd_copy, cmd, COMMAND_SIZE - 1);

    // Split the command into arguments
    split_string(cmd, " ", args, &arg_count);

    // Check for redirection
    for (int i = 0; i < arg_count; i++)
    {
        if (strcmp(args[i], "<") == 0)
        {
            input_file = args[i + 1];
            args[i] = NULL;
            break;
        }
        else if (strcmp(args[i], ">>") == 0)
        {
            output_file = args[i + 1];
            append_mode = 1;
            args[i] = NULL;
            break;
        }
        else if (strcmp(args[i], ">") == 0)
        {
            output_file = args[i + 1];
            append_mode = 0;
            args[i] = NULL;
            break;
        }
    }

    // Check whether the command is internal or external
    // internel commands: cd, echo, export, history, exit, jobs, fg, bg
    int internal = 1;
    if (strcmp(args[0], "cd") == 0)
    {
        do_cd(arg_count, args);
    }
    else if (strcmp(args[0], "pwd") == 0)
    {
        do_pwd();
    }
    else if (strcmp(args[0], "clear") == 0)
    {
        do_clear();
    }
    else if (strcmp(args[0], "echo") == 0)
    {
        do_echo(args[1]);
    }
    else if (strcmp(args[0], "history") == 0)
    {
        do_history();
    }
    else if (strcmp(args[0], "exit") == 0)
    {
        do_exit();
    }
    else if (strcmp(args[0], "jobs") == 0)
    {
        do_jobs();
    }
    else if (strcmp(args[0], "fg") == 0)
    {
        do_fg(atoi(args[1]));
    }
    else if (strcmp(args[0], "bg") == 0)
    {
        do_bg(atoi(args[1]));
    }
    else if (strcmp(args[0], "about") == 0)
    {
        do_about();
    }
    else
    {
        internal = 0;
    }

    pid_t pid = fork();
    if (pid == 0)
    { // Child process
        // Handle input redirection
        if (input_file)
        {
            int fd = open(input_file, O_RDONLY);
            if (fd < 0)
            {
                perror("open");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        // Handle output redirection
        if (output_file)
        {
            int fd;
            if (append_mode)
            {
                fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            }
            else
            {
                fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            if (fd < 0)
            {
                perror("open");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        // externel
        if (!internal)
        {
            if (is_executable(args[0]))
            {
                execv(args[0], args);
            }
            else
            {
                execvp(args[0], args);
            }
            perror("execvp");
            exit(1);
        }
        else
        {
            exit(0);
        }
    }
    else if (pid > 0)
    { // Parent process
        int status;
        if (background)
        {
            // printf("--------------------");

            add_job(pid, cmd_copy, BACKGROUND);
            waitpid(pid, &status, WNOHANG);
            printf("[%d] %d\n", *job_count, pid);
        }
        else
        {
            add_job(pid, cmd_copy, FOREGROUND);
            waitpid(pid, &status, WUNTRACED);
        }
    }
    else
    {
        perror("fork");
    }
}

// Function to execute a pipeline of commands
void execute_pipeline(char **cmds, int cmd_count, int background)
{
    int pipe_fds[2];
    int prev_fd = -1;

    for (int i = 0; i < cmd_count; i++)
    {
        pipe(pipe_fds);

        pid_t pid = fork();
        if (pid == 0)
        { // Child process
            if (prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            if (i < cmd_count - 1)
            {
                dup2(pipe_fds[1], STDOUT_FILENO);
            }
            close(pipe_fds[0]);
            close(pipe_fds[1]);

            execute_command(cmds[i], 0);
            exit(0);
        }
        else if (pid > 0)
        { // Parent process
            if (!background || i == cmd_count - 1)
            {
                waitpid(pid, NULL, 0);
            }
            close(pipe_fds[1]);
            if (prev_fd != -1)
            {
                close(prev_fd);
            }
            prev_fd = pipe_fds[0];
        }
        else
        {
            perror("fork");
        }
    }
}

void init_jobs()
{
    // use shm
    int shm_id = shmget(IPC_CREAT, sizeof(Job) * MAX_JOBS + sizeof(int) * 2, IPC_CREAT | 0666);
    if (shm_id == -1)
    {
        perror("shmget");
        exit(1);
    }
    void *shm = shmat(shm_id, 0, 0);
    jobs = (Job *)shm;
    job_count = (int *)((char *)shm + sizeof(jobs) * MAX_JOBS);

    *job_count = 0;
}

void process_commands(char *input)
{
    char *and_cmds[MAX_CMDS];
    int and_count;

    // Split by ;
    split_string(input, ";", and_cmds, &and_count);

    for (int i = 0; i < and_count; i++)
    {
        char *pipe_cmds[MAX_CMDS];
        int pipe_count;
        int background = 0;

        // Check for background execution
        if (and_cmds[i][strlen(and_cmds[i]) - 1] == '&')
        {
            background = 1;
            and_cmds[i][strlen(and_cmds[i]) - 1] = '\0';
        }

        // Split by |
        split_string(and_cmds[i], "|", pipe_cmds, &pipe_count);

        if (pipe_count == 1)
        {
            execute_command(pipe_cmds[0], background);
        }
        else
        {
            execute_pipeline(pipe_cmds, pipe_count, background);
        }

        // If the last command in the pipeline fails, stop executing further
        if (i < and_count - 1 && WEXITSTATUS(0) != 0)
        {
            break;
        }
    }
}

void init_signal()
{
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = handle_sigchld;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);
    signal(SIGTSTP, handle_sigtstp);
    signal(SIGSTOP, handle_sigtstp);
}

int main(int argc, char **argv)
{
    char *line;
    char command[COMMAND_SIZE];

    printf("yaush: Yet Another Unix Shell\n");
    printf("Type 'exit' to quit\n");

    // initialize the signal and jobs
    init_signal();
    init_jobs();

    while (1)
    {
        set_prompt(command);
        // printf("%s", command);
        line = readline(command);
        if (line == NULL)
        {
            break;
        }
        if (*line)
        {
            add_history(line);
            process_commands(line);
        }
    }
}