ENTRY(_start)

SECTIONS
{
    . = 0x10000;

    .text : {
        *(.text.start)     /* ← _start en premier, obligatoire */
        *(.text)
        *(.text.*)
    }

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        *(.data)
        *(.data.*)
    }

    .bss : {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }
}
