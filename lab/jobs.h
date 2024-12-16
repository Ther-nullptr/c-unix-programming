#ifndef JOBS_H
#define JOBS_H

#include "marcos.h"
#include "header.h"

typedef struct
{
    pid_t pid;
    char command[COMMAND_SIZE];
    int status;
    int type;
    int fg_bg_flag;
} Job;

#endif // JOBS_H