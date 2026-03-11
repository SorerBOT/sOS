#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <types.h>
#include <process_types.h>

void interrupts_setup(void);
void interrupts_set_rsp(const void* _rsp);
const void* interrupts_get_rsp(void);
void interrupts_context_switch();
void interrupts_init_context(void* stack_frame, process_routine_t routine);

#endif /* INTERRUPTS_H */
