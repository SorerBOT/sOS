#ifndef VMM_H
#define VMM_H

#include <types.h>
#include <pmm.h>

#define VMM_KERNEL_MAP_VIRTUAL_OFFSET 0xFFFF800000000000
#define VMM_TRANSLATE_PHYSICAL_TO_KERNEL_MAP(physical_address) (((qword)(physical_address)) + VMM_KERNEL_MAP_VIRTUAL_OFFSET)
#define VMM_TRANSLATE_KERNEL_MAP_TO_PHYSICAL(kernel_map_address) (((qword)(kernel_map_address)) - VMM_KERNEL_MAP_VIRTUAL_OFFSET)

void vmm_setup(void);
void* vmm_create_page_table(void);
void vmm_page_allocate(void* _pml4t);
void vmm_page_free(void* _pml4t);

#endif /* VMM_H */
