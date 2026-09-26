#include "keyboard.h"
#include "screen.h"
#include "shell.h"

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outb(unsigned short port, unsigned char data) {
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

char scancode_to_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void keyboard_handler() {
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);

    // Touches relâchées
    if (scancode & 0x80) {
        outb(0x20, 0x20);
        return;
    }

    char ascii = scancode_to_ascii[scancode];

    if (ascii != 0) {
        // Toute la logique d'affichage est déléguée au shell
        shell_handle_char(ascii);
    }

    outb(0x20, 0x20);
}

void keyboard_sync_cursor() {
    // Plus utilisé avec le shell, mais gardé pour compat
}

void keyboard_init() {
    // Rien pour l'instant
}
