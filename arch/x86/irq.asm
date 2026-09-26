[bits 32]
[global irq1_handler]
[extern keyboard_handler]

irq1_handler:
    pushad
    call keyboard_handler
    popad
    iretd
