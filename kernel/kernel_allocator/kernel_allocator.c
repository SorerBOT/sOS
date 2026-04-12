#include <kernel_allocator.h>
#include <slab_allocator.h>

static void* slab_allocators[16];

void kernel_allocator_setup(void)
{
    for ( size_t i = 3; i < 16; ++i )
    {
        slab_allocators[i] = slab_allocator_init(i << 16);
    }
}
