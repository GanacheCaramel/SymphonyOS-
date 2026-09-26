; boot/boot.s
[org 0x7c00]
[bits 16]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti

    mov si, msg_boot
    call print_string

    mov ax, 0x1000
    mov es, ax
    xor bx, bx
    mov ah, 0x02
    mov al, 30
    mov ch, 0x00
    mov cl, 0x02
    mov dh, 0x00
    mov dl, 0x80
    int 0x13
    jc disk_error

    mov si, msg_ok
    call print_string

    call enable_protected_mode

disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp print_string
.done:
    ret

%INCLUDE "gdt.inc"

msg_boot db "TadOS v0.4 - Loading kernel...", 13, 10, 0
msg_ok db "Kernel loaded. Switching to protected mode...", 13, 10, 0
msg_disk_error db "DISK ERROR!", 13, 10, 0

times 510 - ($ - $$) db 0
dw 0xAA55
