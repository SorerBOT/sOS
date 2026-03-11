#ifndef INTERRUPTS_H
#define INTERRUPTS_H

void interrupts_setup(void);

void interrupts_set_rsp(const void* _rsp);
const void* interrupts_get_rsp(void);
void interrupts_context_switch();

#endif /* INTERRUPTS_H */
