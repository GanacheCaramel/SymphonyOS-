#include "screen.h"
#include "pic.h"
#include "idt.h"
#include "keyboard.h"
#include "shell.h"
#include "banner.h"

extern void irq1_handler();

void default_isr() {
    print_string("EXCEPTION!", 24, 0, 0x0C);
    while(1) { asm volatile("hlt"); }
}

void kernel_main() {
    clear_screen();
    print_banner();

    // 1. IDT
    idt_install();
    for (int i = 0; i < 32; i++) {
        idt_set_gate(i, (unsigned long)default_isr, 0x08, 0x8E);
    }
    idt_set_gate(33, (unsigned long)irq1_handler, 0x08, 0x8E);

    // 2. PIC
    remap_pic();

    // 3. Shell : démarre à la ligne 15, avec le curseur
    shell_init_at(15, 0);

    // 4. Interruptions
    asm volatile("sti");

    while (1) {
        asm volatile("hlt");
    }
}
