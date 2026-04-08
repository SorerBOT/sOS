#include <interrupts.h>
#include <process_manager.h>
#include <process_types.h>
#include <pmm.h>
#include <vmm.h>

#include <stdint.h>
#include <string.h>

#define PROCESS_MANAGER_MAX_PROCESSES 32

static process_control_block_t processes[PROCESS_MANAGER_MAX_PROCESSES];
static size_t processes_count = 0;
static process_id_t running_process_idx = 0;
static bool is_running = false;

process_id_t process_manager_launch_process(process_routine_t routine)
{
    process_id_t new_process_idx = processes_count;

    void* page_table = vmm_create_page_table();
    byte* stack_physical_frame = pmm_frame_alloc();
    vmm_page_bind_to_frame(page_table, page_table);

    byte* stack_kernel_map = VMM_TRANSLATE_PHYSICAL_TO_KERNEL_MAP(stack_physical_frame);
    void* stack_bottom = (stack_kernel_map + PMM_FRAME_SIZE - 1);
    void* process_context = interrupts_init_context((void*)stack_bottom, page_table, routine);

    processes[new_process_idx] = (process_control_block_t)
    {
        .pid = new_process_idx,
        .rsp = process_context 
    };

    ++processes_count;

    return new_process_idx;
}

const process_control_block_t* process_manager_context_switch(process_control_block_t current_control_block)
{
    process_id_t current_pid = process_manager_get_running_process_idx();
    size_t processes_count = process_manager_get_processes_count();
    process_id_t next_pid = (current_pid + 1) % processes_count;

    if ( is_running )
    {
        processes[running_process_idx] = current_control_block;
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
