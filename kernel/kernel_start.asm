[BITS 64]

global kernel_start
extern kernel

kernel_start:
; boot_info_t is still in RDI
    call kernel
