[BITS 64]

global kernel_start
extern kernel

kernel_start:
    call kernel
