; Booting Sequence:
;   - Firmware runs and ensures all electrical devices are OK.
;   - Firmware runs BIOS which are just some code directly embedded
;           on the motherboard and meant to be the entry point of the PC.
;   - BIOS search the first 512 bytes of every drive, and check if they
;           terminate with the magic number: 0x55AA (big endian).
;   - BIOS copy these 512 bytes to address 0x7c00 and JMP to it
;   - This is where our bootloader comes to life.
%define CRLF 0x0D, 0x0A

%define BIOS_INT_PRINT 0x10
%define BIOS_FUNC_PRINT_CHAR 0x0E

%define BIOS_INT_DISK 0x13
%define BIOS_FUNC_CHECK_LBA_SUPPORT 0x41
%define BIOS_FUNC_DISK_READ 0x42

%define REPORT_FAILURE_PREFIX "[ FAILURE ] "
%define REPORT_SUCCESS_PREFIX "[ SUCCESS ] "

%define STAGE_1_ORG 0x7C00
%define STAGE_2_ORG 0x7E00

[BITS 16]
[ORG STAGE_1_ORG]

start:
; We want to ensure CS=0, to change it (or keep it as is) we perform a far jump
    jmp 0x0000:main


main:
; We need to zero some segment base pointers. We mostly want to avoid segmentation, and work with physical addresses
    cli                 ; disable interrupts for bootloader
    xor ax, ax

; defining the stack to be before stage 1 code.
; since it grows backwards, we can set it to STAGE_1_ORG
    mov ss, ax
    mov sp, STAGE_1_ORG
    mov bp, STAGE_1_ORG

; Zero-ing some segment pointers
    mov ds, ax
    mov es, ax
    mov gs, ax

; I want to use LBA addressing
check_bios_support_lba_addressing:
    mov ah, BIOS_FUNC_CHECK_LBA_SUPPORT
    int BIOS_INT_DISK
    jc failed_check_bios_support_lba_addressing

success_check_bios_support_lba_addressing:
    mov si, SUCCESS_LBA_SUPPORT_MSG
    call print_msg
    jmp load_stage_2

load_stage_2:
                        ; BIOS store the current boot driveID in dl. We need not set it ourselves.
    mov si, DAP
    mov ah, BIOS_FUNC_DISK_READ
    int BIOS_INT_DISK   ; First BIOS call to store stage 2 (16KiB) at 0x7E00.
    jc failed_read_disk ; CF==1 means that an error has occurred.
    jmp STAGE_2_ORG     ; jump to the RAM address where we loaded the code

failed_read_disk:
    mov si, FAIL_READ_DISK_MSG
    call print_msg
    jmp error

failed_check_bios_support_lba_addressing:
    mov si, FAIL_LBA_SUPPORT_MSG
    call print_msg
    jmp error


print_msg:
    mov al, [si]
    test al, al
    jz print_finished
    mov ah, BIOS_FUNC_PRINT_CHAR
    int BIOS_INT_PRINT
    inc si
    jmp print_msg    
print_finished:
    ret

error:
    hlt
    jmp $               ; infinite loop. I use it to keep QEMU open



; Defining DAP for BIOS Interrupt 13h which reads from disk and writes to RAM
align 4                 ; Just to be safe, align on 4-byte boundary
DAP:
    db 0x10             ; Packet Size, this tells the BIOS what version of DAP struct we're using
    db 0x00             ; Padding byte. Needs to be reset to 0 if ran in a loop
    dw 0x0040           ; Number of sectors to read

                        ; RAM address to write to is represented by (Segment * 16) + Offset
    dw STAGE_2_ORG      ; Offset, directly after stage 1 which is at STAGE_1_ORG
    dw 0x0000           ; Segment

    dq 0x00000001       ; Disk sector to read from, each sector is 512 bytes.
                        ; the first one contains this file, the next sector will contain stage 2.


FAIL_LBA_SUPPORT_MSG:
    db REPORT_FAILURE_PREFIX, "no LBA support.", CRLF, 0
SUCCESS_LBA_SUPPORT_MSG:
    db REPORT_SUCCESS_PREFIX, "LBA support verified.", CRLF, 0
FAIL_READ_DISK_MSG:
    db REPORT_FAILURE_PREFIX, "couldn't read from disk.", CRLF, 0


; Adding the magic number at the end of the 512 bytes indicates that this is a bootloader
times 510-($-$$) db 0
dw 0xAA55
