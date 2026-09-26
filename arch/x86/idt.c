#include "idt.h"

// Alignement 16 bytes pour être sûr avec lidt
__attribute__((aligned(16))) struct idt_entry idt[256];
struct idt_ptr idtp;

void idt_install() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;
    asm volatile("lidt %0" : : "m"(idtp));
}

void idt_set_gate(int num, unsigned long handler, unsigned short sel, unsigned char flags) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
    idt[num].offset_high = (handler >> 16) & 0xFFFF;
}
