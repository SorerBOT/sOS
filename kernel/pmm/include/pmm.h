#ifndef PMM_H
#define PMM_H

#include <types.h>

#define PMM_FRAME_SIZE 0x200000

void pmm_setup(qword memory_map_address, word memory_map_entries_count);
void* pmm_frame_alloc(void);
void pmm_frame_free(void* ptr);

#endif /* PMM_H */
