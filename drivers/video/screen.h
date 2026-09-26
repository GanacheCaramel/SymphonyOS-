#ifndef SCREEN_H
#define SCREEN_H

void clear_screen();
void print_string(const char *str, int row, int col, char attr);
void print_char_at(char c, int row, int col, char attr);

// Nouveau : gestion du curseur VGA matériel
void cursor_enable(unsigned char start, unsigned char end);
void cursor_disable();
void cursor_move(int row, int col);
void cursor_get_position(int *row, int *col);

#endif
