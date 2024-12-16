#ifndef HANDLE_H
#define HANDLE_H

#include "header.h"

void handle_sigchld(int sig, siginfo_t *info, void *vcontext);
void handle_sigtstp(int sig);

#endif // HANDLE_H