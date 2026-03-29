#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#define MEMORY_MANAGER_FRAME_SIZE 0x200000

void memory_manager_setup(void);

void* memory_manager_frame_alloc(void);

void memory_manager_frame_free(void* ptr);


#endif /* MEMORY_MANAGER_H */
