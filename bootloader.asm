; Booting Sequence:
;   - Firmware runs and ensures all electrical devices are OK.
;   - Firmware runs BIOS which are just some code directly embedded
;           on the motherboard and meant to be the entry point of the PC.
;   - BIOS search the first 512 bytes of every drive, and check if they
;           terminate with the magic number: 0x55AA (big endian).
;   - BIOS copy these 512 bytes to address 0x7c00 and JMP to it
;   - This is where our bootloader comes to life.

BITS 16                 ; 16-bit mode
ORG 0x7C00

dw 0x1111


_start:
call 0x5
move rax, [0x3]

; Magic Number at the end of the 512 bytes indicates that this is a bootloader
times 510-($-$$) db 0

dw 0xAA55
