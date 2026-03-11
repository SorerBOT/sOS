#include <syscall.h>

extern void send_syscall(void* args, word syscall_number);

void syscall_launch_process(process_routine_t routine)
{
    send_syscall(routine, 1);
}
