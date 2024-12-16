#include "header.h"
#include "utils.h"
#include "marcos.h"
#include "jobs.h"
#include "internal_command.h"

extern int *job_count;
extern Job *jobs;

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
        if (jobs[i].type == BACKGROUND)
        {
            printf("[%d] %d %s %s\n", i + 1, jobs[i].pid, jobs[i].command, status);
        }
    }
    for (int i = 0; i < done_count; i++)
    {
        remove_job(done_pid[i]);
    }
}


void do_fg(int job_id)
{
    int bg_count = 0;
    int job_count_val = *job_count;
    // printf("job_id: %d, job_count: %d, jobs[job_id - 1].active: %d\n", job_id, job_count, jobs[job_id - 1].active);
    if (job_count_val == 0)
    {
        fprintf(stderr, "No jobs\n");
        return;
    }
    if (job_id <= 0 || job_id > job_count_val)
    {
        fprintf(stderr, "Invalid job ID\n");
        return;
    }
    for (int i = 0; i < job_count_val; i++)
    {
        if (jobs[i].type == BACKGROUND)
        {
            bg_count++;
        }
        if (bg_count == job_id)
        {
            printf("%s\n", jobs[i].command);
            if (jobs[i].status == SUSPENDED)
            {
                jobs[i].type = FOREGROUND;
                jobs[i].status = RUNNING;
                jobs[i].fg_bg_flag = 1;
                if (kill(jobs[i].pid, SIGCONT) == -1)
                {
                    perror("kill");
                    return;
                }
                int status;
                waitpid(jobs[i].pid, &status, 0);
            }
            else
            {
                jobs[i].type = FOREGROUND;
                jobs[i].status = RUNNING;
                jobs[i].fg_bg_flag = 1;
                int status;
                waitpid(jobs[i].pid, &status, 0);
                remove_job(jobs[i].pid);
            }
        }
    }
    if (bg_count == 0)
    {
        fprintf(stderr, "No background job currently\n");
    }
}


void do_bg(int job_id)
{
    int bg_count = 0;
    int job_count_val = *job_count;
    if (job_count_val == 0)
    {
        fprintf(stderr, "No jobs\n");
        return;
    }
    if (job_id <= 0 || job_id > job_count_val)
    {
        fprintf(stderr, "Invalid job ID\n");
        return;
    }
    for (int i = 0; i < job_count_val; i++)
    {
        if (jobs[i].type == BACKGROUND)
        {
            bg_count++;
        }
        if (bg_count == job_id && jobs[i].status == SUSPENDED)
        {
            // printf("%s\n", jobs[i].command);
            jobs[i].status = RUNNING;
            jobs[i].fg_bg_flag = 1;
            if (kill(jobs[i].pid, SIGCONT) == -1)
            {
                perror("kill");
                return;
            }

            break;
        }
    }

    if (bg_count == 0)
    {
        fprintf(stderr, "No background job currently\n");
    }
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


void do_about()
{
    // colorful print
    printf("\033[1;32m");
    printf("yalsh ");
    printf("\033[0m");
    printf("\033[1;31m");
    printf("Yet Another Unix Shell\n");
    printf("\033[0m");
    printf("\033[1;33mAuthor\033[0m: Ther\n");
    printf("\033[1;34mEmail\033[0m: yujin-wa24@mails.tsinghua.edu.cn\n");

    printf("\033[1;35mSupport built-in commands\033[0m: \n");
    printf("\033[4mexit\033[0m - exit the shell\n");
    printf("\033[4mcd\033[0m [dir] - change the current working directory\n");
    printf("\033[4mecho\033[0m [message] - print the message\n");
    printf("\033[4mhistory\033[0m - show the command history\n");
    printf("\033[4mbg\033[0m [job_id] - continue the job in background\n");
    printf("\033[4mfg\033[0m [job_id] - continue the job in foreground\n");
    printf("\033[4mjobs\033[0m - show the background jobs\n");
    printf("\033[4mabout\033[0m - show the information about the shell\n");
}


void do_pwd()
{
    char cwd[WORKING_DIR_SIZE];
    if (getcwd(cwd, WORKING_DIR_SIZE) == NULL)
    {
        perror("getcwd");
    }
    else
    {
        printf("%s\n", cwd);
    }
}


void do_clear()
{
    printf("\033[H\033[J");
}


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
    {
        if (chdir(argv[1]) == -1)
        {
            printf("cd: %s\n", strerror(errno));
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


void do_exit()
{
    exit(0);
}