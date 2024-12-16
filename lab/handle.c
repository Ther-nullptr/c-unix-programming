#include "handle.h"
#include "header.h"
#include "marcos.h"
#include "jobs.h"
#include "utils.h"

extern int *job_count;
extern Job *jobs;

void handle_sigchld(int sig, siginfo_t *info, void *vcontext)
{
    // printf("handle_sigchld\n");
    int job_count_val = *job_count;
    int i;
    int saved_errno = errno;
    pid_t pid = info->si_pid;
    // find the job
    for (i = 0; i < job_count_val; i++)
    {
        if (jobs[i].pid == pid)
        {
            break;
        }
    }
    if (i < job_count_val)
    {
        // printf("jobs[%d].type: %d, jobs[%d].status: %d\n", i, jobs[i].type, i, jobs[i].status);
        if (jobs[i].type == BACKGROUND && jobs[i].status == RUNNING)
        {
            // printf("state: job[%d] %d %s\n", i + 1, pid, jobs[i].command);
            if (jobs[i].fg_bg_flag)
            {
                jobs[i].status = RUNNING;
                jobs[i].fg_bg_flag = 0;
                // printf("ewojhfeuoihfiuowqhfdiqw[%d] %d %s\n", i + 1, pid, jobs[i].command);
            }
            else
            {
                jobs[i].status = DONE;
                printf("[%d] %d Done %s\n", i + 1, pid, jobs[i].command);
            }
        }
        else if (jobs[i].type == FOREGROUND || (jobs[i].type == BACKGROUND && jobs[i].status == DONE))
        {
            remove_job(pid);
        }
    }
    // delete

    errno = saved_errno;
}

void handle_sigtstp(int sig)
{
    // printf("handle_sigtstp\n");
    int job_count_val = *job_count;
    printf("\n");
    int i;
    // find the job
    for (i = job_count_val; i >= 0; i--)
    {
        if (jobs[i].type == FOREGROUND)
        {
            break;
        }
    }

    if (i >= 0)
    {
        // printf("suspend %s %d\n", jobs[i].command, jobs[i].pid);
        jobs[i].type = BACKGROUND;
        jobs[i].status = SUSPENDED;
        if (kill(jobs[i].pid, SIGSTOP) == -1)
        {
            perror("kill");
        }
    }
}