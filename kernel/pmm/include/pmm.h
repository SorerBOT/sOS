#ifndef PMM_H
#define PMM_H

#define PMM_FRAME_SIZE 0x200000

void pmm_setup(void);

void* pmm_frame_alloc(void);

void pmm_frame_free(void* ptr);


#endif /* PMM_H */
