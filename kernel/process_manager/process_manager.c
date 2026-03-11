#include <process_manager.h>
#include <process_types.h>

#include <console_output.h>
#include <stdint.h>
#include <string.h>

#include "../../arch/x86_64/interrupts/include/isr.h"

#define PROCESS_MANAGER_MAX_PROCESSES 32
#define PROCESS_MANAGER_BASE_STACK_ADDRESS 0x40000000

static process_context_t processes[PROCESS_MANAGER_MAX_PROCESSES];
static size_t processes_count = 0;
static process_id_t running_process_idx = 0;
static bool is_running = false;

void process_manager_launch_process(process_routine_t routine)
{
    process_id_t new_process_idx = processes_count;

    void* stack_frame = (void*) (PROCESS_MANAGER_BASE_STACK_ADDRESS + 4096 * 2 * new_process_idx);
    isr_args_t* context = (isr_args_t*) stack_frame;
   
    memset(context, 0, sizeof(*context));
    context->rip = (qword) routine;
    context->rsp = (qword) stack_frame;
    context->rflags = 0x202;
    context->cs = 16;
    context->ss = 8;

    processes[new_process_idx] = (process_context_t)
    {
        .pid = new_process_idx,
        .rsp = (qword) stack_frame 
    };

    ++processes_count;
}

const process_context_t* process_manager_context_switch(process_id_t next_pid, process_context_t current_context)
{
    if ( is_running )
    {
        processes[running_process_idx] = current_context;
        running_process_idx = next_pid;

        return &processes[running_process_idx];
    }

    else
    {
        is_running = true;
        running_process_idx = 0;
        return &processes[0];
    }
}

process_id_t process_manager_get_running_process_idx(void)
{
    return running_process_idx;
}

size_t process_manager_get_processes_count(void)
{
    return processes_count;
}
