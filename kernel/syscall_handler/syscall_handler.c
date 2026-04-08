#include <syscall_handler.h>
#include <syscall_types.h>
#include <console_output.h>
#include <process_manager.h>
#include <process_types.h>
#include <interrupts.h>

static syscall_handler_t handlers[SYSCALL_COUNT];

static inline void launch_process(const void* rsp, const void* syscall_args)
{
    process_routine_t routine = (process_routine_t) syscall_args;
    process_manager_launch_process(routine);
    interrupts_context_switch();
}

void syscall_handler_handle(const void* rsp, void* syscall_args, qword syscall_number)
{
    if ( syscall_number < SYSCALL_COUNT )
    {
        syscall_handler_t handler = handlers[syscall_number];
        handler(rsp, syscall_args);
    }
    else
    {
        console_output_printf("Invalid syscall number: %llu\n", syscall_number);
    }
}

static syscall_handler_t handlers[SYSCALL_COUNT] =
{
    [SYSCALL_LAUNCH_PROCESS] = launch_process
};
