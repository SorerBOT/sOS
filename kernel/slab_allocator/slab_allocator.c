#include <slab_allocator.h>
#include <pmm.h>
#include <types.h>
#include <console_output.h>

#define SLAB_ALLOCATOR_BITS_IN_QWORD (8 * sizeof(qword))

slab_allocator_t* slab_allocator_init(size_t entry_size)
{
    byte* frame = pmm_frame_alloc();

    if ( frame == NULL )
    {
        console_output_print_blue_screen("Failed to allocate memory for slab allocator\n");
    }

    byte* base_address = frame + 4 * KiB;

    size_t capacity = (2 * MiB - 4 * KiB) / entry_size;

    slab_allocator_t* allocator = (slab_allocator_t*) frame;
    *allocator = (slab_allocator_t)
    {
        .next = NULL,
        .base_address = base_address,
        .entry_size = entry_size,
        .capacity = capacity,
        .items_count = 0
    };

    for ( size_t i = 0; i < capacity / SLAB_ALLOCATOR_BITS_IN_QWORD; ++i )
    {
        allocator->bitmap[i] = 0;
    }

    return allocator;
}

void* slab_allocator_alloc(slab_allocator_t* allocator)
{
    for ( size_t i = 0; i < allocator->capacity / SLAB_ALLOCATOR_BITS_IN_QWORD; ++i )
    {
        qword bitmap_i = allocator->bitmap[i];
        for ( size_t j = 0; j < SLAB_ALLOCATOR_BITS_IN_QWORD; ++j, bitmap_i >>= 1 )
        {
            if ( bitmap_i & 0b1 )
            {
                continue;
            }

            allocator->bitmap[i] |= 0b1 << j;
            return (byte*)allocator->base_address + allocator->entry_size * (i * SLAB_ALLOCATOR_BITS_IN_QWORD + j);
        }
    }

    return NULL;
}
