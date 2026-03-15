#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

void memory_manager_setup(void);

void memory_manager_frame_alloc(void);

void memory_manager_frame_free(void* ptr);


#endif /* MEMORY_MANAGER_H */
