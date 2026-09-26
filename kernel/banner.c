#include "banner.h"
#include "screen.h"

#define COLOR_LOGO   0x0B   // Cyan
#define COLOR_TITLE  0x0F   // Blanc
#define COLOR_LINE   0x0A   // Vert clair
#define COLOR_STATUS 0x0F   // Blanc

void print_logo() {
    print_string("   _____                       __                      ____  _____", 0, 0, COLOR_LOGO);
    print_string("  / ___/__  ______ ___  ____  / /_  ____  ____  __  __/ __ \\/ ___/", 1, 0, COLOR_LOGO);
    print_string("  \\__ \\/ / / / __ `__ \\/ __ \\/ __ \\/ __ \\/ __ \\/ / / / / / /\\__ \\", 2, 0, COLOR_LOGO);
    print_string(" ___/ / /_/ / / / / / / /_/ / / / / /_/ / / / / /_/ / /_/ /___/ /", 3, 0, COLOR_LOGO);
    print_string("/____/\\__, /_/ /_/ /_/ .___/_/ /_/\\____/_/ /_/\\__, /\\____//____/", 4, 0, COLOR_LOGO);
    print_string("     /____/         /_/                      /____/", 5, 0, COLOR_LOGO);
}

void print_banner() {
    print_logo();
    
    // Ligne de séparation
    print_string("=================================================================", 7, 0, COLOR_LINE);
    print_string("              SymphonyOS v0.4 - Protected Mode 32-bit", 8, 0, COLOR_TITLE);
    print_string("=================================================================", 9, 0, COLOR_LINE);
}

void print_status_line() {
    print_string("  [*] Loading IDT...", 11, 0, COLOR_STATUS);
    print_string("  [+] IDT and PIC ready.", 12, 0, COLOR_LINE);
    print_string("  [>] Type 'help' for commands.", 13, 0, COLOR_STATUS);
}
