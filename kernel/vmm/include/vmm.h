#ifndef VMM_H
#define VMM_H

#include <types.h>
#include <pmm.h>

void* vmm_create_page_table(void);
void vmm_setup(void);
void vmm_page_allocate(void* _pml4t);
void vmm_page_free(void* _pml4t);

#endif /* VMM_H */
