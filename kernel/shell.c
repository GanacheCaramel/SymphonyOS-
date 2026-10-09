#include "shell.h"
#include "screen.h"
#include "keyboard.h"
#include "banner.h"

#define CMD_BUFFER_SIZE 256
#define HISTORY_SIZE    16

static char cmd_buffer[CMD_BUFFER_SIZE];
static int  cmd_length = 0;
static int  cursor_row = 0;
static int  cursor_col = 0;

// Historique
static char history[HISTORY_SIZE][CMD_BUFFER_SIZE];
static int  history_count = 0;   // nombre de commandes enregistrées
static int  history_index = 0;   // position courante dans l'historique
static int  history_view = -1;   // -1 = pas en train de naviguer, sinon index

// Comparaison de chaînes
static int str_equals(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return (*a == '\0' && *b == '\0');
}

static int str_starts_with(const char *str, const char *prefix) {
    while (*prefix) {
        if (*str != *prefix) return 0;
        str++; prefix++;
    }
    return 1;
}

static int str_len(const char *s) {
    int n = 0;
    while (s[n]) n++;
    return n;
}

static void str_copy(char *dst, const char *src, int max) {
    int i = 0;
    while (src[i] && i < max - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

// ============ Affichage ============

static void shell_newline() {
    cursor_col = 0;
    cursor_row++;
    if (cursor_row >= 25) {
        //scroll_screen();
        cursor_row = 24;
    }
    cursor_move(cursor_row, cursor_col);
}

static void shell_putchar(char c, char attr) {
    print_char_at(c, cursor_row, cursor_col, attr);
    cursor_col++;
    if (cursor_col >= 80) {
        shell_newline();
    } else {
        cursor_move(cursor_row, cursor_col);
    }
}

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

// Effacer la ligne courante (pour l'historique)
static void shell_clear_current_line() {
    for (int i = 0; i < 80; i++) {
        print_char_at(' ', cursor_row, i, 0x07);
    }
    cursor_col = 0;
    cursor_move(cursor_row, cursor_col);
}

// ============ Historique ============

static void history_add(const char *cmd) {
    if (cmd[0] == '\0') return;
    
    // Éviter les doublons consécutifs
    if (history_count > 0) {
        int last = (history_count - 1) % HISTORY_SIZE;
        if (str_equals(history[last], cmd)) return;
    }
    
    int idx = history_count % HISTORY_SIZE;
    str_copy(history[idx], cmd, CMD_BUFFER_SIZE);
    history_count++;
    history_index = history_count;
    history_view = -1;
}

static const char *history_get(int index) {
    if (index < 0 || index >= history_count) return 0;
    // Si l'historique a débordé, on décale
    if (history_count > HISTORY_SIZE) {
        int offset = history_count - HISTORY_SIZE;
        if (index < offset) return 0;
    }
    return history[index % HISTORY_SIZE];
}

static void history_navigate_up() {
    if (history_count == 0) return;
    
    if (history_view == -1) {
        history_view = history_count - 1;
    } else if (history_view > 0) {
        history_view--;
    } else {
        return;
    }
    
    const char *cmd = history_get(history_view);
    if (!cmd) return;
    
    shell_clear_current_line();
    str_copy(cmd_buffer, cmd, CMD_BUFFER_SIZE);
    cmd_length = str_len(cmd_buffer);
    shell_puts(cmd_buffer, 0x0F);
}

static void history_navigate_down() {
    if (history_view == -1) return;
    
    history_view++;
    if (history_view >= history_count) {
        // Revenir à la ligne vide
        history_view = -1;
        shell_clear_current_line();
        cmd_buffer[0] = '\0';
        cmd_length = 0;
        return;
    }
    
    const char *cmd = history_get(history_view);
    if (!cmd) return;
    
    shell_clear_current_line();
    str_copy(cmd_buffer, cmd, CMD_BUFFER_SIZE);
    cmd_length = str_len(cmd_buffer);
    shell_puts(cmd_buffer, 0x0F);
}

// ============ Commandes ============

static void cmd_help() {
    shell_puts("Commandes disponibles:\n", 0x0F);
    shell_puts("  help      - Affiche cette aide\n", 0x0F);
    shell_puts("  clear     - Efface l'ecran\n", 0x0F);
    shell_puts("  version   - Affiche la version\n", 0x0F);
    shell_puts("  echo X    - Affiche X\n", 0x0F);
    shell_puts("  history   - Affiche l'historique\n", 0x0F);
    shell_puts("  reboot    - Redemarre le systeme\n", 0x0F);
    shell_puts("  (fleches haut/bas pour l'historique)\n", 0x0A);
}

static void cmd_version() {
    shell_puts("SymphonyOS v0.4 - Protected Mode 32-bit\n", 0x0F);
}

static void cmd_echo(const char *args) {
    shell_puts(args, 0x0F);
    shell_puts("\n", 0x0F);
}

static void cmd_history() {
    int start = 0;
    if (history_count > HISTORY_SIZE) start = history_count - HISTORY_SIZE;
    
    for (int i = start; i < history_count; i++) {
        const char *cmd = history_get(i);
        if (cmd) {
            // Numéro
            char num[8];
            int n = i - start + 1;
            int p = 0;
            if (n >= 10) { num[p++] = '0' + (n / 10) % 10; }
            num[p++] = '0' + n % 10;
            num[p] = '\0';
            
            shell_puts("  ", 0x0A);
            shell_puts(num, 0x0A);
            shell_puts("  ", 0x0A);
            shell_puts(cmd, 0x0F);
            shell_puts("\n", 0x0F);
        }
    }
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
    } else if (str_equals(cmd, "history")) {
        cmd_history();
    } else if (str_equals(cmd, "reboot")) {
        cmd_reboot();
    } else {
        shell_puts("Commande inconnue. Tape 'help'.\n", 0x0C);
    }
}

// ============ Interface clavier ============

void shell_handle_char(char c) {
    // Flèches (codes spéciaux)
    if (c == KEY_UP) {
        history_navigate_up();
        return;
    }
    if (c == KEY_DOWN) {
        history_navigate_down();
        return;
    }
    if (c == KEY_LEFT || c == KEY_RIGHT) {
        // Pas encore implémenté
        return;
    }

    if (c == '\n') {
        cmd_buffer[cmd_length] = '\0';
        shell_newline();
        history_add(cmd_buffer);
        shell_execute(cmd_buffer);
        cmd_length = 0;
        cmd_buffer[0] = '\0';
        history_view = -1;

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

void shell_init_at(int row, int col) {
    cmd_length = 0;
    cmd_buffer[0] = '\0';
    cursor_row = row;
    cursor_col = col;
    history_count = 0;
    history_index = 0;
    history_view = -1;
    cursor_move(cursor_row, cursor_col);
    cursor_enable(14, 15);
}

void shell_init() {
    shell_init_at(0, 0);
}
