#include "screen.h"

#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// Ports du contrôleur CRT (VGA)
#define CRT_INDEX 0x3D4
#define CRT_DATA  0x3D5

// Registres du curseur
#define CURSOR_START_REG 0x0A
#define CURSOR_END_REG   0x0B
#define CURSOR_HIGH_REG  0x0E
#define CURSOR_LOW_REG   0x0F

static inline void outb(unsigned short port, unsigned char data) {
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void clear_screen() {
    char *video = (char*)VIDEO_MEMORY;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x07;
    }
}

void print_char_at(char c, int row, int col, char attr) {
    if (row < 0 || row >= SCREEN_HEIGHT) return;
    if (col < 0 || col >= SCREEN_WIDTH) return;
    char *video = (char*)VIDEO_MEMORY;
    int pos = row * SCREEN_WIDTH + col;
    video[pos * 2] = c;
    video[pos * 2 + 1] = attr;
}

void print_string(const char *str, int row, int col, char attr) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\n') {
            row++;
            col = 0;
        } else {
            print_char_at(str[i], row, col, attr);
            col++;
        }
        i++;
    }
}

// ============ Curseur VGA matériel ============

void cursor_enable(unsigned char start, unsigned char end) {
    outb(CRT_INDEX, CURSOR_START_REG);
    outb(CRT_DATA, start);
    outb(CRT_INDEX, CURSOR_END_REG);
    outb(CRT_DATA, end);
}

void cursor_disable() {
    outb(CRT_INDEX, CURSOR_START_REG);
    outb(CRT_DATA, 0x20);  // bit 5 = désactive le curseur
}

void cursor_move(int row, int col) {
    unsigned short pos = row * SCREEN_WIDTH + col;
    
    outb(CRT_INDEX, CURSOR_HIGH_REG);
    outb(CRT_DATA, (pos >> 8) & 0xFF);
    
    outb(CRT_INDEX, CURSOR_LOW_REG);
    outb(CRT_DATA, pos & 0xFF);
}

void cursor_get_position(int *row, int *col) {
    unsigned short pos = 0;
    
    outb(CRT_INDEX, CURSOR_HIGH_REG);
    pos |= ((unsigned short)inb(CRT_DATA)) << 8;
    
    outb(CRT_INDEX, CURSOR_LOW_REG);
    pos |= inb(CRT_DATA);
    
    *row = pos / SCREEN_WIDTH;
    *col = pos % SCREEN_WIDTH;
}
