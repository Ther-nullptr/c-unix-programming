#ifndef INTERNAL_COMMAND_H
#define INTERNAL_COMMAND_H

void do_jobs();
void do_fg(int job_id);
void do_bg(int job_id);
void do_history();
void do_about();
void do_pwd();
void do_clear();
void do_cd(int arg_count, char **args);
void do_echo(char *arg);
void do_exit();

#endif // INTERNAL_COMMAND_H