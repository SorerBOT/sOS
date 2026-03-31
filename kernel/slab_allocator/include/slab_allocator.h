#ifndef SLAB_ALLOCATOR_H
#define SLAB_ALLOCATOR_H

#include <types.h>

typedef struct _slab_allocator_t
{
    struct _slab_allocator_t* next;
    void* base_address;
    size_t entry_size;
    size_t capacity;
    size_t items_count;
    qword bitmap[];
} slab_allocator_t;


void* slab_allocator_init(size_t entry_size);
void* slab_allocator_alloc(void* _allocator);

#endif /* SLAB_ALLOCATOR_H */
