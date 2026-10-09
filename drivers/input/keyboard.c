#include "keyboard.h"
#include "screen.h"
#include "shell.h"

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

// Scancodes spéciaux (make codes)
#define SC_LSHIFT    0x2A
#define SC_RSHIFT    0x36
#define SC_CAPSLOCK  0x3A

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outb(unsigned short port, unsigned char data) {
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

// ============ Tables de scancodes ============

// Table normale (touches non shiftées)
static const char scancode_normal[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Table shiftée (touches avec Shift appuyé)
static const char scancode_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// ============ État du clavier ============

static int shift_pressed = 0;      // Shift gauche ou droit enfoncé
static int capslock_active = 0;    // CapsLock activé (toggle)
static int extended = 0;           // Scancode étendu (0xE0)

// ============ Handler ============

void keyboard_handler() {
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);

    // --- Scancode étendu ---
    if (scancode == 0xE0) {
        extended = 1;
        outb(0x20, 0x20);
        return;
    }

    if (extended) {
        extended = 0;
        // Touches relâchées étendues : on ignore
        if (scancode & 0x80) {
            outb(0x20, 0x20);
            return;
        }
        
        switch (scancode) {
            case 0x48: shell_handle_char(KEY_UP);    break;
            case 0x50: shell_handle_char(KEY_DOWN);  break;
            case 0x4B: shell_handle_char(KEY_LEFT);  break;
            case 0x4D: shell_handle_char(KEY_RIGHT); break;
        }
        outb(0x20, 0x20);
        return;
    }

    // --- Touches relâchées (bit 7 à 1) ---
    if (scancode & 0x80) {
        unsigned char make = scancode & 0x7F;
        
        // Relâchement de Shift
        if (make == SC_LSHIFT || make == SC_RSHIFT) {
            shift_pressed = 0;
        }
        
        outb(0x20, 0x20);
        return;
    }

    // --- Touches spéciales ---
    
    // Shift enfoncé
    if (scancode == SC_LSHIFT || scancode == SC_RSHIFT) {
        shift_pressed = 1;
        outb(0x20, 0x20);
        return;
    }

    // CapsLock (toggle)
    if (scancode == SC_CAPSLOCK) {
        capslock_active = !capslock_active;
        outb(0x20, 0x20);
        return;
    }

    // --- Caractères normaux ---
    char ascii = 0;
    
    // Détermine si on doit utiliser la table shiftée
    // Logique : 
    //   - Shift appuyé → shift table
    //   - CapsLock activé + lettre → shift table
    //   - CapsLock + Shift + lettre → normal table (les deux s'annulent)
    int use_shift = shift_pressed;
    
    if (capslock_active) {
        // CapsLock n'affecte que les lettres
        char c = scancode_normal[scancode];
        if (c >= 'a' && c <= 'z') {
            use_shift = !use_shift;  // inverse pour les lettres
        }
    }
    
    ascii = use_shift ? scancode_shift[scancode] : scancode_normal[scancode];

    if (ascii != 0) {
        shell_handle_char(ascii);
    }

    outb(0x20, 0x20);
}

void keyboard_sync_cursor() { }
void keyboard_init() { }
