#ifndef PROCESS_TYPES_H
#define PROCESS_TYPES_H

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
    qword rsp;
    qword rdi;
    qword rsi;
    qword rip;
    qword rax;
    qword rbx;
    qword rcx;
    qword rdx;
} process_registers_t;

typedef struct
{
    process_registers_t registers;
} process_context_t;

typedef void(*process_routine_t)(void*);

typedef size_t process_id_t;

#endif /* PROCESS_TYPES_H */
