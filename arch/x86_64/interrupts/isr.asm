extern isr_handler

global isr_wrapper
align 16

%define FAKE_ERROR_CODE 0
%define FAKE_ERROR_CODE 0

%macro generate_isr_wrapper_without_error_code 1
global isr_wrapper_%1
isr_wrapper_%1:
    push FAKE_ERROR_CODE
    push %1
    jmp isr_wrapper_common
%endmacro

%macro generate_isr_wrapper_with_error_code 1
global isr_wrapper_%1
isr_wrapper_%1:
    push %1
    jmp isr_wrapper_common
%endmacro

isr_wrapper_common:
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

; RETRIEVING GENERAL PURPOSE REGISTERS & CLEARING ERROR CODE + INTERRUPT NUMBER FROM THE STACK
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

    add rsp, 16             ; accounting for the error code, and the interrupt number that we puhsed onto the stack

    iretq
