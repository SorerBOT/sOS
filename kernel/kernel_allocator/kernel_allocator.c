#include "console_output.h"
#include <kernel_allocator.h>
#include <slab_allocator.h>
#include <math_extended.h>
#include <pmm.h>

#define KERNEL_ALLOCATR_MIN_ALLOCATION_SIZE_LOG_2 3
#define KERNEL_ALLOCATOR_MAX_ALLOCATION_SIZE_LOG_2 20

// to make things simple, slab_allocators[i] = the allocation for 2 << i (instead of zero indexing)
static void* slab_allocators[KERNEL_ALLOCATOR_MAX_ALLOCATION_SIZE_LOG_2+1];

void kernel_allocator_setup(void)
{
    for ( size_t i = KERNEL_ALLOCATR_MIN_ALLOCATION_SIZE_LOG_2; i <= KERNEL_ALLOCATOR_MAX_ALLOCATION_SIZE_LOG_2; ++i )
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

    if ( log2 > KERNEL_ALLOCATOR_MAX_ALLOCATION_SIZE_LOG_2 )
    {
        console_output_print_blue_screen("Kernel Allocator Error: allocation of size: %zu exceeds maximum allocation size of: %zu (%zu > %zu)\n", size, 2 << KERNEL_ALLOCATOR_MAX_ALLOCATION_SIZE_LOG_2, log2, KERNEL_ALLOCATOR_MAX_ALLOCATION_SIZE_LOG_2);
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    void* appropriate_allocator = slab_allocators[log2];

    return slab_allocator_allocate(appropriate_allocator);
}

void kernel_allocator_free(void* address)
{
    void* original_allocator = PMM_GET_FRAME_ADDRESS(address);
    slab_allocator_free(original_allocator, address);
}
