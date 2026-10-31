#include <console_output.h>
#include <kernel_allocator.h>
#include <slab_allocator.h>
#include <math_extended.h>
#include <vmm.h>

#define KERNEL_ALLOCATR_MIN_ALLOCATION_SIZE_LOG_2 5
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
    /*
     * Mode 1: use slab allocator for allocations smaller or equal to VMM_PAGE_SIZE / 2 
     */
    if ( size <= VMM_PAGE_SIZE / 2 )
    {
        size_t log2 = math_extended_round_up_to_log_two(size);

        /*
         * I round up allocations smaller than 2 << KERNEL_ALLOCATR_MIN_ALLOCATION_SIZE_LOG_2 bytes to 2 << KERNEL_ALLOCATR_MIN_ALLOCATION_SIZE_LOG_2.
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

    /*
     * Mode 2: allocate an entire page.
     */
    else if ( size <= VMM_PAGE_SIZE )
    {
        return vmm_allocate_kernel_page();
    }

    /*
     * Mode 3: reserve a large contiguous address range and several physical frames and then map between them.
     */
    else
    {
            console_output_print_blue_screen("Kernel Allocator Error: allocations larger than VMM_PAGE_SIZE are not supported yet.\n");
            while ( 1 )
            {
                __asm__ volatile("hlt");
            }
    }
}

void kernel_allocator_free(void* address)
{
    for ( size_t i = KERNEL_ALLOCATR_MIN_ALLOCATION_SIZE_LOG_2; i <= KERNEL_ALLOCATOR_MAX_ALLOCATION_SIZE_LOG_2; ++i )
    {
        if ( slab_allocator_is_own_address(slab_allocators[i], address) )
        {
            return slab_allocator_free(slab_allocators[i], address);
        }
    }

    vmm_free_kernel_page(address);
    /*
     * missing the unimplemented mode 3.
     */
    return;

    console_output_print_blue_screen("Kernel Allocator Error: address %p was not allocated by the kernel allocator.\n", address);
    while ( 1 )
    {
        __asm__ volatile("hlt");
    }
}
