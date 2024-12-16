#ifndef MARCOS_H
#define MARCOS_H

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
#define FOREGROUND 0
#define BACKGROUND 1

#endif