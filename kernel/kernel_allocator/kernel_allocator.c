#include "console_output.h"
#include <kernel_allocator.h>
#include <slab_allocator.h>
#include <math_extended.h>
#include <pmm.h>

#define KERNEL_ALLOCATR_MIN_ALLOCATION_SIZE_LOG_2 3

static void* slab_allocators[16];

void kernel_allocator_setup(void)
{
    for ( size_t i = KERNEL_ALLOCATR_MIN_ALLOCATION_SIZE_LOG_2; i < 16; ++i )
    {
        slab_allocators[i] = slab_allocator_init(2 << i);
    }
}

void* kernel_allocator_allocate(size_t size)
{
    size_t log2 = math_extended_round_up_to_log_two(size);

    /*
     * I round up allocations smaller than 8 bytes to 8 bytes.
     */
    log2 += (log2 < KERNEL_ALLOCATR_MIN_ALLOCATION_SIZE_LOG_2) * (KERNEL_ALLOCATR_MIN_ALLOCATION_SIZE_LOG_2 - log2);

    void* appropriate_allocator = slab_allocators[log2];

    return slab_allocator_allocate(appropriate_allocator);
}

void kernel_allocator_free(void* address)
{
    void* original_allocator = (void*)(((qword)address) & ~(PMM_FRAME_SIZE - 1));
    slab_allocator_free(original_allocator, address);
}
