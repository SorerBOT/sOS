#include <kernel_allocator.h>
#include <slab_allocator.h>
#include <math_extended.h>

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
    log2 += (log2 < KERNEL_ALLOCATR_MIN_ALLOCATION_SIZE_LOG_2) * (KERNEL_ALLOCATR_MIN_ALLOCATION_SIZE_LOG_2 - log2);


}
