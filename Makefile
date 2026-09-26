# ============================================================
#  SymphonyOS - Makefile
# ============================================================

CC      := clang
LD      := ld.lld
ASM     := nasm
OBJCOPY := llvm-objcopy

CFLAGS  := --target=i386-elf \
           -ffreestanding -fno-stack-protector -fno-stack-check \
           -fno-pic -fno-builtin -m32 -mno-red-zone \
           -Wall -Wextra \
           -Ikernel/include \
           -Ilibc/include \
           -Idrivers/video \
           -Idrivers/input \
           -Iarch/x86 \
           -Ikernel

ASFLAGS := -f elf32

BIN     := bin

C_SOURCES := \
    kernel/kernel.c \
    kernel/shell.c \
    kernel/banner.c \
    arch/x86/idt.c \
    arch/x86/pic.c \
    drivers/video/screen.c \
    drivers/input/keyboard.c \
    libc/string.c \
    libc/stdio.c

ASM_SOURCES := \
    arch/x86/start.asm \
    arch/x86/irq.asm

C_OBJECTS   := $(patsubst %.c,$(BIN)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst %.asm,$(BIN)/%.o,$(ASM_SOURCES))
OBJECTS     := $(C_OBJECTS) $(ASM_OBJECTS)

.PHONY: all clean run

all: $(BIN)/symphony.img

$(BIN)/boot.bin: arch/x86/boot/boot.asm arch/x86/boot/gdt.inc
	@mkdir -p $(dir $@)
	$(ASM) -f bin -I arch/x86/boot/ $< -o $@

$(BIN)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASFLAGS) $< -o $@

$(BIN)/kernel.elf: $(OBJECTS) linker.ld
	$(LD) -m elf_i386 -static -nostdlib -T linker.ld $(OBJECTS) -o $@

$(BIN)/kernel.bin: $(BIN)/kernel.elf
	$(OBJCOPY) -O binary $< $@

$(BIN)/symphony.img: $(BIN)/boot.bin $(BIN)/kernel.bin
	dd if=/dev/zero of=$@ bs=1024 count=1440 2>/dev/null
	dd if=$(BIN)/boot.bin of=$@ conv=notrunc 2>/dev/null
	dd if=$(BIN)/kernel.bin of=$@ conv=notrunc bs=512 seek=1 2>/dev/null

run: all
	qemu-system-i386 -display curses -drive format=raw,file=$(BIN)/symphony.img

clean:
	rm -rf $(BIN)
