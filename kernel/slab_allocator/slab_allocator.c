#include <slab_allocator.h>
#include <pmm.h>
#include <types.h>
#include <console_output.h>

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

    return allocator;
}
