[BITS 16]
[ORG 0x7E00]

start:
    mov ah, 0x0E    ; BIOS Teletype Output
    mov al, 'H'
    int 0x10
    mov al, 'e'
    int 0x10
    mov al, 'l'
    int 0x10

    mov al, 'l'
    int 0x10
    mov al, 'o'
    int 0x10
    mov al, ' '
    int 0x10
    mov al, 'M'
    int 0x10
    mov al, 'a'
    int 0x10
    mov al, 'n'
    int 0x10
    mov al, '!'
    int 0x10

    jmp $      ; Hang forever
