%define CRLF 0x0D, 0x0A
%define GDT_BASE_ADDRESS
%define GDT_SEGMENT_DATA_SELECTOR 0x08
%define GDT_SEGMENT_CODE_SELECTOR 0x10

%define REPORT_FAILURE_PREFIX "[ FAILURE ] "
%define REPORT_SUCCESS_PREFIX "[ SUCCESS ] "
%define REPORT_MSG_PREFIX   "[ MESSAGE ] "

%define BIOS_INT_PRINT 0x10
%define BIOS_FUNC_PRINT_CHAR 0x0E

%define BIOS_INT_DISK 0x13
%define BIOS_FUNC_DISK_READ 0x42

%define STAGE_1_ORG 0x7C00

%define SECTORS_PER_SEGMENT 128

[BITS 16]

global GDT_HEADER
global BASE_PAGE_TABLE_ADDRESS

extern page_table_setup
extern update_gdt

start:
    cli


    xor ax, ax
    mov ds, ax


    mov ss, ax
    mov sp, STAGE_1_ORG
    mov bp, STAGE_1_ORG


    mov si, STAGE_1_COMPLETED_MSG
    call print_msg


    call load_kernel
    call enable_a20_fastgate
    call setup_gdt


    mov si, INFO_JUMPING_TO_PROTECTED_MODE_MSG
    call print_msg


    jmp enable_and_jump_to_protected_mode

load_kernel:
    mov si, DAP
    mov ah, BIOS_FUNC_DISK_READ
    int BIOS_INT_DISK   ; First BIOS call to store stage 2 (16KiB) at 0x7E00.
    jc fail_read_disk   ; CF==1 means that an error has occurred.

    mov si, SUCCESS_LOAD_KERNEL_MSG
    call print_msg
    ret

fail_read_disk:
    mov si, FAIL_READ_DISK_MSG
    call print_msg
    jmp error

print_msg:
    mov al, [si]
    test al, al
    jz print_msg_finished
    mov ah, BIOS_FUNC_PRINT_CHAR
    int BIOS_INT_PRINT
    inc si
    jmp print_msg

print_msg_finished:
    ret

error:
    jmp $               ; infinite loop. I use it to keep QEMU open

enable_a20_fastgate:
; ENABLING a20 USING FAST GATE
    in al, 0x92
    or al, 0x02
    out 0x92, al

    mov si, SUCCESS_ENABLE_A20_MSG
    call print_msg

    ret

setup_gdt:
; SETTING GDT
; in order to set the GDT, we need to point GDTR to the GDT_HEADER label defined below
    lgdt [GDT_HEADER]       ; loading GDT_HEADER into gdtr

    mov si, SUCCESS_SETUP_GDT_MSG
    call print_msg

    ret

enable_and_jump_to_protected_mode:
; SETTING CR0.PE (MOVING TO PROTECTED MODE)
    mov eax,cr0
    or eax, 1
    mov cr0, eax

; THE LEAP OF FAITH (WEARING THE PROTECTION)
    jmp GDT_SEGMENT_CODE_SELECTOR:protected_mode_start


align 4                     ; Just to be safe, align on 4-byte boundary
DAP:
    db 0x10                 ; Packet Size, this tells the BIOS what version of DAP struct we're using
    db 0x00                 ; Padding byte. Needs to be reset to 0 if ran in a loop

DAP_SECTORS_COUNT:
    dw SECTORS_PER_SEGMENT  ; Number of sectors to read

DAP_OFFSET:                 ; RAM address to write to is represented by (Segment * 16) + Offset
    dw 0x0000               ; Offset, directly at the beginning of the segment

DAP_SEGMENT:
    dw 0x1000               ; Segment this is exactly 64KiB.

    dq 0x00000042           ; Disk sector to read from, each sector is 512 bytes.
                            ; the first one contains stage 1, the next 32 contian stage 2
                            ; and afterwards its the kernel
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

STAGE_1_COMPLETED_MSG:
    db REPORT_SUCCESS_PREFIX, "stage 1 completed.", CRLF, 0

FAIL_READ_DISK_MSG:
    db REPORT_FAILURE_PREFIX, "couldn't read from disk.", CRLF, 0

SUCCESS_LOAD_KERNEL_MSG:
    db REPORT_SUCCESS_PREFIX, "finished loading kernel.", CRLF, 0

SUCCESS_ENABLE_A20_MSG:
    db REPORT_SUCCESS_PREFIX, "finished enabling a20.", CRLF, 0

SUCCESS_SETUP_GDT_MSG:
    db REPORT_SUCCESS_PREFIX, "finished setting up gdt.", CRLF, 0

INFO_JUMPING_TO_PROTECTED_MODE_MSG:
    db REPORT_MSG_PREFIX, "jumping to protected mode.", CRLF, 0


[BITS 32]

%define LONG_MODE_MSR 0xC0000080
%define KERNEL_ORG 0x10000

%define CONSOLE_IO_FAILURE 0
%define CONSOLE_IO_SUCCESS 1

%define ASCII_LINE_FEED 0x0a

extern console_io_report

; FOR THE TIME BEING, I DON'T CHECK WHETHER LONG MODE IS AVAILABLE.
; I KNOW THAT IT IS ON MY HARDWARE, AND I CARE MUCH MORE ABOUT MAKING
; SOMETHING COOL, THAN I DO CARE ABOUT COMPATIBILITY
protected_mode_start:
    cld ; CLEARING DIRECTION FLAG

    cli ; DISABLING INTERRUPTS (GOING TO SET STACK SEGMENT)

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
    call load_page_table
    call update_gdt
    call enable_pae_paging
    call toggle_long_mode_paging

    jmp enable_and_jump_to_long_mode

load_page_table:
; TELLING THE CPU WHERE THE PAGE TABLE IS
    mov eax, [BASE_PAGE_TABLE_ADDRESS] 
    mov cr3, eax

    push CONSOLE_IO_SUCCESS
    push SUCCESS_LOAD_PAGE_TABLE_MSG
    call console_io_report
    add esp, 8

    ret

enable_pae_paging:
; TELLING THE CPU THAT I WANNA USE PAE PAGING (ONLY TAKES EFFECT WHEN PAGING WOULD BE ENABLED)
    mov eax, cr4
    or eax, 1 << 5 ; THE PAE BIT IS THE SIXTH BIT
    mov cr4, eax

    push CONSOLE_IO_SUCCESS
    push SUCCESS_ENABLE_PAE_PAGING_MSG
    call console_io_report
    add esp, 8

    ret

toggle_long_mode_paging:
; TOGGLING LONG MODE PAGING
    mov ecx, LONG_MODE_MSR
    rdmsr
    or eax, 1 << 8 ; LONG MODE PAGING BIT IS THE NINTH BIT
    wrmsr

    push CONSOLE_IO_SUCCESS
    push SUCCESS_TOGGLE_LONG_MODE_PAGING_MSG
    call console_io_report
    add esp, 8

    ret

enable_and_jump_to_long_mode:
; LONG MODE
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

; FAR JUMP, THE LEAP OF FAITH x2
    jmp GDT_SEGMENT_CODE_SELECTOR:long_mode_start


BASE_PAGE_TABLE_ADDRESS:
    dd 0x1000000


SUCCESS_LOAD_PAGE_TABLE_MSG:
    db "loaded page table.",  0

SUCCESS_ENABLE_PAE_PAGING_MSG:
    db "enabled PAE paging." , 0

SUCCESS_TOGGLE_LONG_MODE_PAGING_MSG:
    db "toggled long mode paging." , 0

[BITS 64]
long_mode_start:
    cli ; DISABLING INTERRUPTS (GOIGN TO SET STACK SEGMENT)

; LETTING THE KERNEL TAKE CONTROL
    jmp KERNEL_ORG
