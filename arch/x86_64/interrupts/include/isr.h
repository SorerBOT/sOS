#ifndef ISR_H
#define ISR_H

#include <types.h>

typedef struct
{
    qword r15;
    qword r14;
    qword r13;
    qword r12;
    qword r11;
    qword r10;
    qword r9;
    qword r8;
    qword rbp;
    qword rsi;
    qword rdx;
    qword rcx;
    qword rbx;
    qword rax;
    qword rdi;
} isr_registers_t;

typedef struct
{
    isr_registers_t general_registers;
    qword isr_number;
    qword error_code;
    qword rip;
    qword cs;
    qword rflags;
    qword rsp;
    qword ss;
} isr_args_t;

void isr_handler(isr_args_t* args);

#endif /* ISR_H */
