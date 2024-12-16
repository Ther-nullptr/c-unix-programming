#ifndef UTILS_H
#define UTILS_H

#include "header.h"

int is_executable(char *path);
void add_job(pid_t pid, const char *command, int type);
void remove_job(pid_t pid);
void split_string(char *str, char *delim, char **output, int *count);

#endif // UTILS_H