[BITS 16]
[ORG 0x7E00]

%define CLRF 0x0D, 0x0A
%define OS_STATUS "[sOS]"

start:
    xor ax, ax
    mov ds, ax
    mov si, msg_stage_2
print_msg:
    mov al, [si]
    test al, al
    jz print_msg_finished
    mov ah, 0x0E    ; BIOS Teletype Output
    int 0x10
    inc si
    jmp print_msg


print_msg_finished:
    jmp $      ; Hang forever


msg_stage_2 db OS_STATUS, ": booting...", CLRF, OS_STATUS, ": stage 1 completed...", CLRF, OS_STATUS, ": running stage 2...", 0
