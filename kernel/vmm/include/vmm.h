#ifndef VMM_H
#define VMM_H

#include <types.h>
#include <pmm.h>

#define VMM_PAGE_SIZE PMM_FRAME_SIZE
#define VMM_PDPT_COUNT_IN_PML4T (VMM_PAGE_SIZE / sizeof(PDPT_t*))
#define VMM_PDT_COUNT_IN_PDPT (VMM_PAGE_SIZE / sizeof(PDT_t*))
#define VMM_PT_COUNT_IN_PDT (VMM_PAGE_SIZE / sizeof(PT_t*))
#define VMM_PAGE_COUNT_IN_PT (VMM_PAGE_SIZE / sizeof(Frame_t*))

typedef struct
{
    byte data[VMM_PAGE_SIZE];
} Frame_t;

typedef struct
{
    Frame_t* pages[VMM_PAGE_COUNT_IN_PT];
} PT_t;

typedef struct
{
    PT_t* pts[VMM_PT_COUNT_IN_PDT];
} PDT_t;

typedef struct
{
    PDT_t* pdts[VMM_PDT_COUNT_IN_PDPT];
} PDPT_t;

typedef struct
{
    PDPT_t* pdpts[VMM_PDPT_COUNT_IN_PML4T];
} PML4T_t;


PML4T_t* vmm_create_page_table(void);
void* vmm_page_allocate(void);
void vmm_page_free(void);


#endif /* VMM_H */
