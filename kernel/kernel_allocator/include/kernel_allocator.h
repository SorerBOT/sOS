#ifndef KERNEL_ALLOCATOR_H
#define KERNEL_ALLOCATOR_H

#include <types.h>

void kernel_allocator_setup(void);
void* kernel_allocator_allocate(size_t size);
void kernel_allocator_free(void* address);

#endif /* KERNEL_ALLOCATOR_H */
