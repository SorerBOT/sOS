[BITS 16]

global start
global GDT_HEADER
global BASE_PAGE_TABLE_ADDRESS

extern page_table_setup
extern update_gdt
;extern kernel

%define CRLF 0x0D, 0x0A
%define OS_STATUS "[sOS]"
%define GDT_BASE_ADDRESS
%define GDT_SEGMENT_DATA_SELECTOR 0x08
%define GDT_SEGMENT_CODE_SELECTOR 0x10

start:
    xor ax, ax
    mov ds, ax
align 4                 ; Just to be safe, align on 4-byte boundary
DAP:
    db 0x10             ; Packet Size, this tells the BIOS what version of DAP struct we're using
    db 0x00             ; Padding byte. Needs to be reset to 0 if ran in a loop
    dw 0x0040           ; Number of sectors to read

                        ; RAM address to write to is represented by (Segment * 16) + Offset
    dw 0x0000           ; Offset, directly after 16KiB
    dw 0x1000           ; Segment this is exactly 16 KiB.

    dq 0x00000042       ; Disk sector to read from, each sector is 512 bytes.
                        ; the first one contains stage 1, the next 32 contian stage 2
                        ; and afterwards its the kernel

    mov si, DAP
    mov ah, 0x42
    int 0x13            ; First BIOS call to store stage 2 (16KiB) at 0x7E00.
    jc error            ; CF==1 means that an error has occurred.
    jmp stage_2_after_loading_kernel

error:
    jmp $               ; infinite loop. I use it to keep QEMU open

stage_2_after_loading_kernel:
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
    jmp GDT_SEGMENT_CODE_SELECTOR:protected_mode_start

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

%define LONG_MODE_MSR 0xC0000080
%define KERNEL_ORG 0x10000

BASE_PAGE_TABLE_ADDRESS:
    dd 0x1000000


; FOR THE TIME BEING, I DON'T CHECK WHETHER REAL MODE IS AVAILABLE.
; I KNOW THAT IT IS ON MY HARDWARE, AND I CARE MUCH MORE ABOUT MAKING
; SOMETHING COOL, THAN I DO CARE ABOUT COMPATIBILITY
protected_mode_start:
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

; PREPARE LONG MODE
    call page_table_setup
    call update_gdt

; TELLING THE CPU WHERE THE PAGE TABLE IS
    mov eax, [BASE_PAGE_TABLE_ADDRESS] 
    mov cr3, eax

; TELLING THE CPU THAT I WANNA USE PAE PAGING (ONLY TAKES EFFECT WHEN PAGING WOULD BE ENABLED)
    mov eax, cr4
    or eax, 1 << 5 ; THE PAE BIT IS THE SIXTH BIT
    mov cr4, eax

; TOGGLING LONG MODE PAGING
    mov ecx, LONG_MODE_MSR
    rdmsr

    or eax, 1 << 8 ; LONG MODE PAGING BIT IS THE NINTH BIT
    wrmsr

; LONG MODE
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

; FAR JUMP, THE LEAP OF FAITH x2
    jmp GDT_SEGMENT_CODE_SELECTOR:long_mode_start

[BITS 64]
long_mode_start:
    cli ; DISABLING INTERRUPTS (GOIGN TO SET STACK SEGMENT)

; LETTING THE KERNEL TAKE CONTROL
    jmp KERNEL_ORG
