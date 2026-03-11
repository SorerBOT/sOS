#ifndef APPLICATION_MANAGER_H
#define APPLICATION_MANAGER_H

#include <process_types.h>

void process_manager_launch_process(process_routine_t routine);
const process_context_t* process_manager_context_switch(process_id_t next_pid, process_context_t current_context);
process_id_t process_manager_get_running_process_idx(void);
size_t process_manager_get_processes_count(void);

#endif /* APPLICATION_MANAGER_H */
