#include <stdio.h>
#include <string.h>
#include <features.h>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
// #include <asm-generic/siginfo.h>
#include <fcntl.h>
#include <sys/shm.h>

#include <readline/readline.h>
#include <readline/history.h>

#define MAX_ARGS 128
#define HOST_NAME_SIZE 100
#define WORKING_DIR_SIZE 200
#define TIME_SIZE 100
#define COMMAND_SIZE 300
#define LAST_DIR_SIZE 100
#define HISTORY_FILE "~/yaush_history"
#define HISTORY_SIZE 100
#define MAX_CMDS 10
#define MAX_JOBS 20

// marcos for job status
#define RUNNING 0
#define SUSPENDED 1
#define DONE 2

// marcos for job type
#define FG 0
#define BG 1

char last_dir[LAST_DIR_SIZE];

typedef struct
{
    pid_t pid;
    char command[COMMAND_SIZE];
    int status;
    int type;
} Job;

Job* jobs;//[MAX_JOBS];
int* job_count;

// Function to check if a command is executable
int is_executable(char *path)
{
    struct stat sb;
    return stat(path, &sb) == 0 && sb.st_mode & S_IXUSR;
}

Job* add_job(pid_t pid, const char *command, int type)
{
    int job_count_val = *job_count;
    if (job_count_val < MAX_JOBS)
    {
        jobs[job_count_val].pid = pid;
        strncpy(jobs[job_count_val].command, command, COMMAND_SIZE - 1);
        jobs[job_count_val].command[COMMAND_SIZE - 1] = '\0';
        jobs[job_count_val].status = RUNNING;
        jobs[job_count_val].type = type;
        printf("%d: add job %s\n", *job_count, command);
        *job_count = job_count_val + 1;
        printf("job count: %d\n", *job_count);
    }
    else
    {
        fprintf(stderr, "Too many jobs\n");
    }

    return (jobs + job_count_val);
}

void remove_job(pid_t pid)
{
    int i;
    int job_count_val = *job_count;
    for (i = 0; i < job_count_val; i++)
    {
        if (jobs[i].pid == pid)
        {
            break;
        }
    }
    // printf("remove job %s\n", jobs[i].command);
    if (i < job_count_val)
    {
        for (int j = i; j < job_count_val; j++)
        {
            printf("move job %s to %s\n", jobs[j + 1].command, jobs[j].command);
            jobs[j] = jobs[j + 1];

        }
        *job_count = job_count_val - 1;
        printf("job count: %d\n", *job_count);
    }
}

void do_jobs()
{
    int done_count = 0;
    int done_pid[MAX_JOBS];
    int job_count_val = *job_count;
    if (job_count_val == 0)
    {
        fprintf(stderr, "No jobs\n");
        return;
    }
    for (int i = 0; i < job_count_val; i++)
    {
        char *status;
        switch (jobs[i].status)
        {
        case RUNNING:
            status = "RUNNING";
            break;
        case SUSPENDED:
            status = "SUSPENDED";
            break;
        case DONE:
            status = "DONE";
            done_pid[done_count++] = jobs[i].pid;
            break;
        default:
            status = "UNKNOWN";
            break;
        }
        if (jobs[i].type == BG)
        {
            printf("[%d] %d %s %s\n", i + 1, jobs[i].pid, jobs[i].command, status);
        }

    }
    for (int i = 0; i < done_count; i++)
    {
        printf("remove job %d, %s\n", done_pid[i], jobs[i].command);
        remove_job(done_pid[i]);
    }
}

void do_fg(int job_id)
{
    // printf("job_id: %d, job_count: %d, jobs[job_id - 1].active: %d\n", job_id, job_count, jobs[job_id - 1].active);
    if (*job_count == 0)
    {
        fprintf(stderr, "No jobs\n");
        return;
    }
    if (job_id <= 0 || job_id > *job_count)
    {
        fprintf(stderr, "Invalid job ID\n");
        return;
    }
    pid_t pid = jobs[job_id - 1].pid;
    jobs[job_id - 1].status = RUNNING;
    if (kill(pid, SIGCONT) == -1)
    {
        perror("kill");
        return;
    }
    int status;
    waitpid(pid, &status, 0);
}

void do_bg(int job_id)
{
    if (*job_count == 0)
    {
        fprintf(stderr, "No jobs\n");
        return;
    }
    if (job_id <= 0 || job_id > *job_count)
    {
        fprintf(stderr, "Invalid job ID\n");
        return;
    }
    pid_t pid = jobs[job_id - 1].pid;
    jobs[job_id - 1].status = RUNNING;
    if (kill(pid, SIGCONT) == -1)
    {
        perror("kill");
        return;
    }
    printf("[%d] %d\n", job_id, pid);
}

void do_history()
{
    HIST_ENTRY **h = history_list();
    if (h)
    {
        int i = 0;
        while (h[i])
        {
            printf("%d: %s\n", i, h[i]->line);
            i++;
        }
    }
}

// Signal handler for cleaning up finished background jobs
void handle_sigchld(int sig, siginfo_t* info, void* vcontext)
{
    (void)sig;
    int i;
    int saved_errno = errno;
    pid_t pid = info->si_pid;
    // find the job
    for (i = 0; i < *job_count; i++)
    {
        if (jobs[i].pid == pid)
        {
            break;
        }
    }
    if (i < *job_count)
    {
        if (jobs[i].type == BG && jobs[i].status == RUNNING)
        {
            jobs[i].status = DONE;
        }
        else if (jobs[i].type == BG && jobs[i].status == SUSPENDED)
        {
            // do nothing
        }
        else
        {
            remove_job(pid);
        }
    }

    errno = saved_errno;
}

void handle_sigtstp(int sig)
{
    (void)sig;
    printf("\n");
    int i;
    // find the job
    for (i = *job_count; i >=0 ; i--)
    {
        if (jobs[i].type == FG)
        {
            break;
        }
    }

    if (i >= 0)
    {
        jobs[i].type = BG;
        jobs[i].status = SUSPENDED;
        if (kill(jobs[i].pid, SIGSTOP) == -1)
        {
            perror("kill");
        }
    }
}

void split_string(char *str, char *delim, char **output, int *count)
{
    char *token = strtok(str, delim);
    *count = 0;
    while (token != NULL)
    {
        output[(*count)++] = token;
        token = strtok(NULL, delim);
    }
    output[*count] = NULL;
}

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
    sprintf(prompt, "[%s@%s %s] (%s)\n $ ", pwp->pw_name, host, cwd, time_str);
}

// built-in commands: exit, cd, echo, export, history, bg, fg, jobs

void do_cd(int argc, char **argv)
{
    if (argc == 1)
    { // cd
        if (chdir(getenv("HOME")) == -1)
        {
            printf("cd: %s\n", strerror(errno));
        }
    }
    else if (argc == 2)
    { // cd -
        if (strcmp(argv[1], "-") == 0)
        {
            if (chdir(last_dir) == -1)
            {
                printf("cd: %s\n", strerror(errno));
            }
        }
        else
        { // cd dir
            if (chdir(argv[1]) == -1)
            {
                printf("cd: %s\n", strerror(errno));
            }
        }
    }
    else
    {
        printf("Usage: cd [dir]\n");
    }
}

void do_echo(char *message)
{
    printf("%s\n", message);
}

void do_export(char *var)
{
    char *value = getenv(var);
    if (value != NULL)
    {
        printf("%s=%s\n", var, value);
    }
}

void do_exit()
{
    exit(0);
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

        // Check whether the command is internal or external
        // internel commands: cd, echo, export, history, exit, jobs, fg, bg
        if (strcmp(args[0], "cd") == 0)
        {
            do_cd(arg_count, args);
            exit(0);
        }
        else if (strcmp(args[0], "echo") == 0)
        {
            do_echo(args[1]);
            exit(0);
        }
        else if (strcmp(args[0], "export") == 0)
        {
            do_export(args[1]);
            exit(0);
        }
        else if (strcmp(args[0], "history") == 0)
        {
            do_history();
            exit(0);
        }
        else if (strcmp(args[0], "exit") == 0)
        {
            do_exit();
            exit(0);
        }
        else if (strcmp(args[0], "jobs") == 0)
        {
            do_jobs();
            exit(0);
        }
        else if (strcmp(args[0], "fg") == 0)
        {
            do_fg(atoi(args[1]));
            exit(0);
        }
        else if (strcmp(args[0], "bg") == 0)
        {
            do_bg(atoi(args[1]));
            exit(0);
        }

        // externel
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
    else if (pid > 0)
    { // Parent process
        if (background)
        {
            // printf("--------------------");
            add_job(pid, cmd, BG);
            waitpid(pid, NULL, WNOHANG);
            printf("[%d] %d\n", *job_count, pid);
        }
        else
        {
            add_job(pid, cmd, FG);
            waitpid(pid, NULL, WUNTRACED);
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
    int shm_id = shmget((key_t)1234, sizeof(Job) * MAX_JOBS + sizeof(int), IPC_CREAT | 0666);
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

    // Split by &&
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

int main(int argc, char **argv)
{
    char *line;
    char command[COMMAND_SIZE];


    printf("yaush: Yet Another Unix Shell\n");
    printf("Type 'exit' to quit\n");

    // build a history list

    // Set up signal handler for SIGCHLD
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGCHLD, &sa, NULL); // when receive

    signal(SIGTSTP, handle_sigtstp); // when receive ctrl+z
    signal(SIGSTOP, handle_sigtstp);
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
            // process_commands(line);
            process_commands(line);
        }
        // add_history(line);
        
    }
}