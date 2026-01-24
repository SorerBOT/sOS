[BITS 16]

%define CRLF 0x0D, 0x0A
%define OS_STATUS "[sOS]"
%define GDT_BASE_ADDRESS
%define GDT_SEGMENT_DATA_SELECTOR 0x08
%define GDT_SEGMENT_CODE_SELECTOR 0x10

start:
    xor ax, ax
    mov ds, ax
    mov si, msg_stage_2
print_msg:
    mov al, [si]
    test al, al
    jz print_msg_finished
    mov ah, 0x0E            ; BIOS interrupt for printing a character
    int 0x10
    inc si
    jmp print_msg
print_msg_finished:
prepare_protected_mode:
    cli                     ; disabling interrupts

; ENABLING a20 USING FAST GATE
    in al, 0x92
    or al, 0x02
    out 0x92, al

; SETTING GDT
; in order to set the GDT, we need to point GDTR to the GDT_HEADER label defined below
    lgdt [GDT_HEADER]       ; loading GDT_HEADER into gdtr

; SETTING CR0.PE (MOVING TO PROTECTED MODE)
    mov eax,cr0
    or eax, 1
    mov cr0, eax

; THE LEAP OF FAITH (WEARING THE PROTECTION)
    jmp GDT_SEGMENT_CODE_SELECTOR:PROTECTED_MODE_START

align 8
GDT_START:
GDT_DATA:
    dq 0x00000000               ; the first entry must be null

ENTRY_SEGMENT_DATA:
    dw 0xFFFF                   ; lower part limit
    dw 0x0000                   ; lower part base
    db 0x00                     ; middle part base
    db 0b10010010               ; access byte
    db 0b11000000 | 0b00001111  ; 0b00001111 = the upper part limit, 0b11000000 = flags
    db 0x00                     ; upper part base

ENTRY_SEGMENT_CODE:
    dw 0xFFFF                   ; lower part limit
    dw 0x0000                   ; lower part base
    db 0x00                     ; middle part base
    db 0b10011010               ; access byte
    db 0b11000000 | 0b00001111  ; 0b00001111 = the upper part limit, 0b11000000 = flags
    db 0x00                     ; upper part base
GDT_END:

GDT_HEADER:
    dw GDT_END - GDT_START - 1  ; limit: the maximum offset allowed within the GDT
    dd GDT_START                ; base: the base address of the GDT

msg_stage_2 db OS_STATUS, ": booting...", CRLF, OS_STATUS, ": stage 1 completed...", CRLF, OS_STATUS, ": running stage 2...", 0


[BITS 32]
PROTECTED_MODE_START:
    cld ; CLEARING DIRECTION FLAG

    cli ; DISABLING INTERRUPTS (GOIGN TO SET STACK SEGMENT)

; MAKING ALL SEGMENTS POINT TO THE GDT ENTRY OF THE DATA SEGMENT
    mov ax, GDT_SEGMENT_DATA_SELECTOR
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

; SETTING THE STACK POINTERS
    mov esp, 0x90000
    mov ebp, esp

; ENTER STAGE 3
    extern bootloader_stage_3
    call bootloader_stage_3
