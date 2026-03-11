#include <interrupts.h>
#include <types.h>
#include <process_manager.h>
#include <process_types.h>

#include "include/idt.h"
#include "include/pic.h"

static const void* rsp;

extern void interrupts_enable(void);

void interrupts_setup(void)
{
    idt_setup();
    pic_remap();
    interrupts_enable();
}

void interrupts_set_rsp(const void* _rsp)
{
    rsp = _rsp;
}

const void* interrupts_get_rsp(void)
{
    return rsp;
}

void interrupts_context_switch()
{
    process_context_t current_context =
    {
        .pid = process_manager_get_running_process_idx(),
        .rsp = rsp
    };

    const process_context_t* new_context = process_manager_context_switch(current_context);

    interrupts_set_rsp((const void*) new_context->rsp);
}
