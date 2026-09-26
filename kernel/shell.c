#include "shell.h"
#include "screen.h"
#include "keyboard.h"
#include "banner.h"

#define CMD_BUFFER_SIZE 256

static char cmd_buffer[CMD_BUFFER_SIZE];
static int  cmd_length = 0;
static int  cursor_row = 0;
static int  cursor_col = 0;

// Comparaison de chaînes
static int str_equals(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return (*a == '\0' && *b == '\0');
}

// Vérifie si `str` commence par `prefix`
static int str_starts_with(const char *str, const char *prefix) {
    while (*prefix) {
        if (*str != *prefix) return 0;
        str++; prefix++;
    }
    return 1;
}

// Passe à la ligne suivante (avec scroll simple)
static void shell_newline() {
    cursor_col = 0;
    cursor_row++;
    if (cursor_row >= 25) {
        cursor_row = 0;
        clear_screen();
    }
    cursor_move(cursor_row, cursor_col);
}

// Écrit un caractère à la position du curseur et avance
static void shell_putchar(char c, char attr) {
    print_char_at(c, cursor_row, cursor_col, attr);
    cursor_col++;
    if (cursor_col >= 80) {
        shell_newline();
    } else {
        cursor_move(cursor_row, cursor_col);
    }
}

// Écrit une chaîne
static void shell_puts(const char *str, char attr) {
    while (*str) {
        if (*str == '\n') {
            shell_newline();
            str++;
        } else {
            shell_putchar(*str++, attr);
        }
    }
}

// ============ Commandes ============

static void cmd_help() {
    shell_puts("Commandes disponibles:\n", 0x0F);
    shell_puts("  help      - Affiche cette aide\n", 0x0F);
    shell_puts("  clear     - Efface l'ecran\n", 0x0F);
    shell_puts("  version   - Affiche la version\n", 0x0F);
    shell_puts("  echo X    - Affiche X\n", 0x0F);
    shell_puts("  reboot    - Redemarre le systeme\n", 0x0F);
}

static void cmd_version() {
    shell_puts("SymphonyOS v0.4 - Protected Mode 32-bit\n", 0x0F);
}

static void cmd_echo(const char *args) {
    shell_puts(args, 0x0F);
    shell_puts("\n", 0x0F);
}

static void cmd_reboot() {
    shell_puts("Redemarrage...\n", 0x0F);
    for (volatile int i = 0; i < 10000000; i++);
    asm volatile("outb %0, %1" : : "a"((unsigned char)0xFE), "Nd"((unsigned short)0x64));
    while (1) { asm volatile("hlt"); }
}

// ============ Exécution ============

void shell_execute(const char *cmd) {
    while (*cmd == ' ') cmd++;
    if (*cmd == '\0') return;

    if (str_equals(cmd, "help")) {
        cmd_help();
    } else if (str_equals(cmd, "clear")) {
        clear_screen();
        print_banner();
        print_status_line();
        cursor_row = 15;
        cursor_col = 0;
        cursor_move(cursor_row, cursor_col);
    } else if (str_equals(cmd, "version")) {
        cmd_version();
    } else if (str_starts_with(cmd, "echo ")) {
        cmd_echo(cmd + 5);
    } else if (str_equals(cmd, "echo")) {
        shell_puts("\n", 0x0F);
    } else if (str_equals(cmd, "reboot")) {
        cmd_reboot();
    } else {
        shell_puts("Commande inconnue. Tape 'help'.\n", 0x0C);
    }
}

// ============ Interface clavier ============

void shell_handle_char(char c) {
    if (c == '\n') {
        cmd_buffer[cmd_length] = '\0';
        shell_newline();
        shell_execute(cmd_buffer);
        cmd_length = 0;

    } else if (c == '\b') {
        if (cmd_length > 0) {
            cmd_length--;
            if (cursor_col > 0) {
                cursor_col--;
            } else if (cursor_row > 0) {
                cursor_row--;
                cursor_col = 79;
            }
            print_char_at(' ', cursor_row, cursor_col, 0x07);
            cursor_move(cursor_row, cursor_col);
        }
    } else {
        if (cmd_length < CMD_BUFFER_SIZE - 1) {
            cmd_buffer[cmd_length] = c;
            shell_putchar(c, 0x0F);
            cmd_length++;
        }
    }
}

// Démarrage à une position spécifique (après le banner)
void shell_init_at(int row, int col) {
    cmd_length = 0;
    cursor_row = row;
    cursor_col = col;
    cursor_move(cursor_row, cursor_col);
    cursor_enable(14, 15);
}

// Version par défaut (démarre en haut à gauche)
void shell_init() {
    shell_init_at(0, 0);
}
