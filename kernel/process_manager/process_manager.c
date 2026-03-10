#include <process_manager.h>
#include <process_types.h>

#include <console_output.h>

#define PROCESS_MANAGER_MAX_PROCESSES 32

static process_context_t processes[PROCESS_MANAGER_MAX_PROCESSES];
static size_t processes_count = 1;
static process_id_t running_process_idx = 0;

void process_manager_launch_process(process_routine_t routine)
{
    process_context_t context =
    {
        .registers =
        {
            .r15 = 0,
            .r14 = 0,
            .r13 = 0,
            .r12 = 0,
            .r11 = 0,
            .r10 = 0,
            .r9 = 0,
            .r8 = 0,
            .rbp = 0,
            .rsp = 0,
            .rdi = 0,
            .rsi = 0,
            .rip = (qword) routine,
            .rax = 0,
            .rbx = 0,
            .rcx = 0,
            .rdx = 0,
        }
    };

    process_id_t new_process_idx = processes_count;

    processes[processes_count++] = context;
}

const process_context_t* process_manager_context_switch(process_id_t next_pid, process_context_t current_context)
{
    processes[running_process_idx] = current_context;
    running_process_idx = next_pid;

    return &processes[running_process_idx];
}

process_id_t process_manager_get_running_process_idx(void)
{
    return running_process_idx;
}

size_t process_manager_get_processes_count(void)
{
    return processes_count;
}
