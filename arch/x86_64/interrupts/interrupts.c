#include "include/idt.h"
#include "include/pic.h"
#include "include/isr.h"

#include <types.h>
#include <string.h>

#include <interrupts.h>
#include <process_manager.h>
#include <process_types.h>

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
    process_control_block_t current_control_block =
    {
        .pid = process_manager_get_running_process_idx(),
        .rsp = rsp
    };

    const process_control_block_t* new_context = process_manager_context_switch(current_control_block);

    interrupts_set_rsp((const void*) new_context->rsp);
}

void* interrupts_init_context(void* stack_frame, process_routine_t routine)
{
    isr_args_t* context = (isr_args_t*) (((byte*)stack_frame) - 5 * sizeof(isr_args_t) - 1);
   
    memset(context, 0, sizeof(*context));

    context->rip = (qword) routine;
    context->rsp = (qword) stack_frame;
    context->rflags = 0x202;
    context->cs = 16;
    context->ss = 8;

    return context;
}
