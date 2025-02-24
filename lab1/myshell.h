#ifndef MYSHELL_H
#define MYSHELL_H

void print_prompt();
void execute_cd(char *args[]);
void execute_clr();
void execute_dir(char *args[]);
void execute_environ();
void execute_echo(char *args[]);
void execute_help();
void execute_pause();
void execute_quit();
void execute_external_command(char *args[]);

#endif
