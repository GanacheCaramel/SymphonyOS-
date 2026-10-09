#ifndef SHELL_H
#define SHELL_H

void shell_init();
void shell_init_at(int row, int col);
void scroll_screen();
void shell_handle_char(char c);
void shell_execute(const char *cmd);

#endif
