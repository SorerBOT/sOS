#ifndef VMM_H
#define VMM_H

#include <types.h>
#include <pmm.h>

#define VMM_PAGE_SIZE PMM_FRAME_SIZE
#define VMM_ENTRIES_COUNT_IN_LEVEL 512

typedef struct
{
    void* frames[VMM_ENTRIES_COUNT_IN_LEVEL];
} PDT_t;

typedef struct
{
    PDT_t* pdts[VMM_ENTRIES_COUNT_IN_LEVEL];
} PDPT_t;

typedef struct
{
    PDPT_t* pdpts[VMM_ENTRIES_COUNT_IN_LEVEL];
} PML4T_t;


PML4T_t* vmm_create_page_table(void);
void* vmm_page_allocate(PML4T_t* pml4t);
void vmm_page_free(PML4T_t* pml4t);


#endif /* VMM_H */
