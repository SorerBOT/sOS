#include <syscall_handler.h>
#include <syscall_types.h>
#include <console_output.h>

static syscall_handler_t handlers[SYSCALL_COUNT];

static inline void launch_process(void* syscall_args)
{
    console_output_printf("Launching process ASD:ASDAS?D\n");
}

void syscall_handler_handle(void* syscall_args, qword syscall_number)
{
    if ( syscall_number < SYSCALL_COUNT )
    {
        syscall_handler_t handler = handlers[syscall_number];
        handler(syscall_args);
    }
    else
    {
        console_output_printf("Invalid syscall number\n");
    }
}

static syscall_handler_t handlers[SYSCALL_COUNT] =
{
    [SYSCALL_LAUNCH_PROCESS] = launch_process
};
