#ifndef VMM_H
#define VMM_H

#include <types.h>
#include <pmm.h>

#define VMM_PAGE_SIZE PMM_FRAME_SIZE
#define VMM_KERNEL_MAP_VIRTUAL_OFFSET 0xFFFF800000000000
#define VMM_TRANSLATE_PHYSICAL_TO_KERNEL_MAP(physical_address) ((void*)(((byte*)(physical_address)) + VMM_KERNEL_MAP_VIRTUAL_OFFSET))
#define VMM_TRANSLATE_KERNEL_MAP_TO_PHYSICAL(kernel_map_address) ((void*)(((byte*)(kernel_map_address)) - VMM_KERNEL_MAP_VIRTUAL_OFFSET))

void vmm_setup(void);
void* vmm_create_page_table(void);
void vmm_page_bind_to_frame(void* _pml4t, void* frame);
void vmm_page_allocate(void* _pml4t);
void vmm_page_free(void* _pml4t);

void* vmm_allocate_kernel_page(void);
void vmm_free_kernel_page(void* page);

#endif /* VMM_H */
