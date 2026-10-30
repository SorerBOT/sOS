#include <slab_allocator.h>
#include <pmm.h>
#include <types.h>
#include <console_output.h>
#include <vmm.h>

#define SLAB_ALLOCATOR_BITS_IN_QWORD (8 * sizeof(qword))

static slab_allocator_t slab_allocator_for_allocating_slab_allocators;

static inline void slab_allocator_init_internal(size_t entry_size, slab_allocator_t* allocator)
{
    byte* frame = VMM_TRANSLATE_PHYSICAL_TO_KERNEL_MAP(pmm_frame_alloc());

    if ( frame == NULL )
    {
        console_output_print_blue_screen("Failed to allocate memory for slab allocator\n");
        while (1)
        {
            __asm__("hlt");
        }
    }

    size_t capacity = (2 * MiB) / entry_size;

    *allocator = (slab_allocator_t)
    {
        .next = NULL,
        .base_address = frame,
        .entry_size = entry_size,
        .capacity = capacity,
        .items_count = 0
    };

    for ( size_t i = 0; i < capacity / SLAB_ALLOCATOR_BITS_IN_QWORD; ++i )
    {
        allocator->bitmap[i] = 0;
    }
}

void* slab_allocator_init(size_t entry_size)
{
    if ( entry_size <= 16 )
    {
        console_output_print_blue_screen("Slab Allocator Error: cannot create a slab allocator with entry size lesser than 16, as it causes the bitmap to be at least 4KiB in size which makes my life difficult.\n");
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    if ( slab_allocator_for_allocating_slab_allocators.base_address == NULL )
    {
        slab_allocator_init_internal(4 * KiB, &slab_allocator_for_allocating_slab_allocators);
    }

    slab_allocator_t* allocator = slab_allocator_allocate(&slab_allocator_for_allocating_slab_allocators);
    slab_allocator_init_internal(entry_size, allocator);

    return allocator;
}

void* slab_allocator_allocate(void* _allocator)
{
    slab_allocator_t* allocator = _allocator;

    for ( size_t i = 0; i < allocator->capacity / SLAB_ALLOCATOR_BITS_IN_QWORD; ++i )
    {
        qword bitmap_i = allocator->bitmap[i];
        for ( size_t j = 0; j < SLAB_ALLOCATOR_BITS_IN_QWORD; ++j, bitmap_i >>= 1 )
        {
            if ( bitmap_i & 0b1 )
            {
                continue;
            }

            size_t entry_idx = (i * SLAB_ALLOCATOR_BITS_IN_QWORD + j);

            if ( entry_idx >= allocator->capacity )
            {
                break;
            }

            allocator->bitmap[i] |= 0b1 << j;
            return (byte*)allocator->base_address + allocator->entry_size * entry_idx;
        }
    }

    console_output_print_blue_screen("slab allocator ran out of space\n");
    while (1)
    {
        __asm__("hlt");
    }

    return NULL;
}

void slab_allocator_free(void* _allocator, void* address)
{
    slab_allocator_t* allocator = _allocator;

    size_t entry_idx = (((byte*)address) - ((byte*)allocator->base_address)) / allocator->entry_size;

    size_t bitmap_idx = entry_idx / SLAB_ALLOCATOR_BITS_IN_QWORD;

    size_t entry_idx_within_bitmap = entry_idx & (SLAB_ALLOCATOR_BITS_IN_QWORD - 1);

    allocator->bitmap[bitmap_idx] &= 0b0 << entry_idx_within_bitmap;
}
