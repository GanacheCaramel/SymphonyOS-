#ifndef KEYBOARD_H
#define KEYBOARD_H

// Codes spéciaux envoyés au shell (pour les flèches)
#define KEY_UP    0x01
#define KEY_DOWN  0x02
#define KEY_LEFT  0x03
#define KEY_RIGHT 0x04

void keyboard_handler();
void keyboard_sync_cursor();
void keyboard_init();

#endif
