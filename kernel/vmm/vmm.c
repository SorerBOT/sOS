#include <vmm.h>
#include <pmm.h>
#include <string.h>
#include <slab_allocator.h>

/* I don't really like this. */
#define VMM_IS_PRESENT(address) (((qword)address) & 0x1)
#define VMM_FLAG_PRESENT                    (0b1ULL)
#define VMM_FLAG_READ_WRITE                 (0b1ULL >> 1)
#define VMM_FLAG_USER                       (0b1ULL >> 2)
#define VMM_FLAG_SUPERVISOR                 (0b0ULL >> 2)
#define VMM_FLAG_WRITE_THROUGH              (0b0ULL >> 3)
#define VMM_FLAG_CACHE_DISABLE              (0b0ULL >> 4)
#define VMM_FLAG_ACCESSED                   (0b0ULL >> 5)
#define VMM_FLAG_DIRTY                      (0b0ULL >> 6)
#define VMM_FLAG_PAGE_SIZE_HUGE             (0b1ULL >> 7)
#define VMM_FLAG_PAGE_SIZE_NORMAL           (0b0ULL >> 7)
#define VMM_FLAG_GLOBAL                     (0b1ULL >> 8)
#define VMM_FLAG_LOCAL                      (0b0ULL >> 8)
#define VMM_FLAG_EXECUTE_ALLOW              (0b0ULL >> 62)
#define VMM_FLAG_EXECUTE_DISALLOW           (0b1ULL >> 62)

#define VMM_FLAGS_USER_TABLE (VMM_FLAG_PRESENT | VMM_FLAG_READ_WRITE | VMM_FLAG_USER                \
                                | VMM_FLAG_WRITE_THROUGH | VMM_FLAG_CACHE_DISABLE                   \
                                | VMM_FLAG_ACCESSED | VMM_FLAG_DIRTY | VMM_FLAG_PAGE_SIZE_NORMAL    \
                                | VMM_FLAG_LOCAL | VMM_FLAG_EXECUTE_ALLOW)

#define VMM_FLAGS_USER_PAGE (VMM_FLAG_PRESENT | VMM_FLAG_READ_WRITE | VMM_FLAG_USER                 \
                                | VMM_FLAG_WRITE_THROUGH | VMM_FLAG_CACHE_DISABLE                   \
                                | VMM_FLAG_ACCESSED | VMM_FLAG_DIRTY | VMM_FLAG_PAGE_SIZE_HUGE      \
                                | VMM_FLAG_LOCAL | VMM_FLAG_EXECUTE_ALLOW)

#define VMM_FLAGS_KERNEL_TABLE (VMM_FLAG_PRESENT | VMM_FLAG_READ_WRITE | VMM_FLAG_SUPERVISOR        \
                                | VMM_FLAG_WRITE_THROUGH | VMM_FLAG_CACHE_DISABLE                   \
                                | VMM_FLAG_ACCESSED | VMM_FLAG_DIRTY | VMM_FLAG_PAGE_SIZE_NORMAL    \
                                | VMM_FLAG_GLOBAL | VMM_FLAG_EXECUTE_ALLOW)

#define VMM_FLAGS_KERNEL_PAGE (VMM_FLAG_PRESENT | VMM_FLAG_READ_WRITE | VMM_FLAG_SUPERVISOR         \
                                | VMM_FLAG_WRITE_THROUGH | VMM_FLAG_CACHE_DISABLE                   \
                                | VMM_FLAG_ACCESSED | VMM_FLAG_DIRTY | VMM_FLAG_PAGE_SIZE_HUGE      \
                                | VMM_FLAG_GLOBAL | VMM_FLAG_EXECUTE_ALLOW)

#define VMM_PAGE_TABLE_SIZE (4 * KiB)

static void* slab_allocator = NULL;

void vmm_setup(void)
{
    if ( slab_allocator == NULL )
    {
        slab_allocator = slab_allocator_init(VMM_PAGE_TABLE_SIZE);
    }
}

PML4T_t* vmm_create_page_table(void)
{
    PML4T_t* pml4t = slab_allocator_allocate(slab_allocator);

    if ( pml4t == NULL )
    {
        return NULL;
    }

    memset(pml4t, 0, sizeof(PML4T_t));

    return pml4t;
}




void* vmm_page_allocate(PML4T_t* pml4t)
{
    for ( size_t i = 0; i < VMM_ENTRIES_COUNT_IN_LEVEL; ++i )
    {
        if ( VMM_IS_PRESENT(pml4t->pdpts[i]) == false )
        {
            PDPT_t* pdpt = slab_allocator_allocate(slab_allocator);
            pml4t->pdpts[i] = (PDPT_t*)(((qword)pdpt) | VMM_FLAGS_USER_TABLE);
        }

        for ( size_t j = 0; j < VMM_ENTRIES_COUNT_IN_LEVEL; ++j )
        {
            PDPT_t* pdpt_clean = (PDPT_t*)((qword)pml4t->pdpts[i] & ~VMM_FLAGS_USER_TABLE);
            if ( VMM_IS_PRESENT(pdpt_clean->pdts[j]) == false )
            {
                PDT_t* pdt = slab_allocator_allocate(slab_allocator);
                pml4t->pdpts[i]->pdts[j] = (PDT_t*)(((qword)pdt) | VMM_FLAGS_USER_TABLE);
            }

            else
            {
                for ( size_t k = 0; k < VMM_ENTRIES_COUNT_IN_LEVEL; ++k )
                {
                    PDT_t* pdt_clean = (PDT_t*)(((qword)pml4t->pdpts[i]->pdts[j]) & ~VMM_FLAGS_USER_TABLE);
                    if ( VMM_IS_PRESENT(pdt_clean->frames[k]) )
                    {
                        continue;
                    }

                    void* frame = pmm_frame_alloc();
                    pml4t->pdpts[i]->pdts[j] = (void*)(((qword)frame) | VMM_FLAGS_USER_PAGE);
                    return (void*)1;
                }
            }
        }
    }

    return NULL;
}


void vmm_page_free(PML4T_t* pml4t);
