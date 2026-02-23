#ifndef ISR_H
#define ISR_H

#include <types.h>

enum
{
    ISR_BREAKPOINT = 3
};

typedef struct
{
    qword isr_number;
    qword error_code;
    qword instruction_pointer;
    qword code_segment;
    qword rflags;
    qword stack_pointer;
    qword stack_segment;
} isr_args_t;

void isr_handler(isr_args_t* args);

#endif /* ISR_H */
