extern isr_handler

.globl isr_wrapper
.align 16

isr_wrapper:
;   PUSHING GENERAL PURPOSE REGISTERS
    push rdi
    lea rdi, [rsp + 8]      ; upon receiving an interrupt the CPU pushes some data onto the stack
                            ; I want to send it to the C handler, so I'm making rdi point to it.
                            ; ****Note that the stack grows upside down, meaning that the CPU's struct
                            ; begins on the last stack address, so this already points to it.
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    cld                     ; sysV ABI requirement.

    call isr_handler

; RETRIEVING GENERAL PURPOSE REGISTERS
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rdi

    iretq
