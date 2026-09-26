[bits 32]
[global _start]
[extern kernel_main]

section .text.start
_start:
    mov esp, 0x9C000
    call kernel_main
.halt:
    hlt
    jmp .halt
