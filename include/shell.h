#ifndef SHELL_H
#define SHELL_H

extern char input[256];
extern int input_pos;

extern int edit_mode;

void execute_command();
void handle_edit_line();

#endif
