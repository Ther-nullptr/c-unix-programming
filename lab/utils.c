#include "utils.h"
#include "marcos.h"
#include "header.h"
#include "jobs.h"

extern int *job_count;
extern Job *jobs;

// Function to check if a command is executable
int is_executable(char *path)
{
    struct stat sb;
    return stat(path, &sb) == 0 && sb.st_mode & S_IXUSR;
}

void add_job(pid_t pid, const char *command, int type)
{
    int job_count_val = *job_count;
    if (job_count_val < MAX_JOBS)
    {
        jobs[job_count_val].pid = pid;
        strncpy(jobs[job_count_val].command, command, COMMAND_SIZE - 1);
        jobs[job_count_val].command[COMMAND_SIZE - 1] = '\0';
        jobs[job_count_val].status = RUNNING;
        jobs[job_count_val].type = type;
        jobs[job_count_val].fg_bg_flag = 0;
        *job_count = job_count_val + 1;
    }
    else
    {
        fprintf(stderr, "Too many jobs\n");
    }
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
            jobs[j] = jobs[j + 1];
        }
        *job_count = job_count_val - 1;
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
