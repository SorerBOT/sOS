#include <syscall_dispatcher.h>
#include <syscall_types.h>

extern void syscall_dispatcher_dispatch(void* syscall_args, qword syscall_number);

void syscall_dispatcher_launch_process(process_routine_t routine)
{
    syscall_dispatcher_dispatch(routine, SYSCALL_LAUNCH_PROCESS);
}
