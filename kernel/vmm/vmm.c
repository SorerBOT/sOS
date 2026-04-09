#include <vmm.h>
#include <pmm.h>
#include <string.h>
#include <slab_allocator.h>
#include <console_output.h>

#define VMM_PAGE_SIZE PMM_FRAME_SIZE
#define VMM_ENTRIES_COUNT_IN_LEVEL 512

/* I don't really like this. */
#define VMM_KERNEL_PML4T_BASE (VMM_TRANSLATE_PHYSICAL_TO_KERNEL_MAP(0x1000000))
#define VMM_PML4T_HIGHER_HALF_OFFSET 256
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

typedef struct
{
    void* frames[VMM_ENTRIES_COUNT_IN_LEVEL];
} PDT_t;

typedef struct
{
    PDT_t* pdts[VMM_ENTRIES_COUNT_IN_LEVEL];
} PDPT_t;

typedef struct
{
    PDPT_t* pdpts[VMM_ENTRIES_COUNT_IN_LEVEL];
} PML4T_t;

static void* slab_allocator = NULL;
static const PML4T_t* kernel_pml4t = (PML4T_t*) VMM_KERNEL_PML4T_BASE;

void vmm_setup(void)
{
    if ( slab_allocator == NULL )
    {
        slab_allocator = slab_allocator_init(VMM_PAGE_TABLE_SIZE);
    }
}

void* vmm_create_page_table(void)
{
    PML4T_t* pml4t = slab_allocator_allocate(slab_allocator);

    if ( pml4t == NULL )
    {
        return NULL;
    }

    memset(pml4t, 0, sizeof(PML4T_t));

    for ( size_t i = VMM_PML4T_HIGHER_HALF_OFFSET; i < VMM_ENTRIES_COUNT_IN_LEVEL; ++i )
    {
        pml4t->pdpts[i] = kernel_pml4t->pdpts[i];
    }

    return pml4t;
}

void vmm_page_bind_to_frame(void* _pml4t, void* frame)
{
    PML4T_t* pml4t = _pml4t;
    for ( size_t i = 0; i < VMM_ENTRIES_COUNT_IN_LEVEL; ++i )
    {
        if ( VMM_IS_PRESENT(pml4t->pdpts[i]) == false )
        {
            byte* pdpt_physical = VMM_TRANSLATE_KERNEL_MAP_TO_PHYSICAL(slab_allocator_allocate(slab_allocator));
            pml4t->pdpts[i] = (PDPT_t*)(((qword)pdpt_physical) | VMM_FLAGS_USER_TABLE);
        }

        for ( size_t j = 0; j < VMM_ENTRIES_COUNT_IN_LEVEL; ++j )
        {
            byte* pdpt_clean_physical = (byte*)((qword)pml4t->pdpts[i] & ~VMM_FLAGS_USER_TABLE);
            PDPT_t* pdpt_clean_kernel_map = VMM_TRANSLATE_PHYSICAL_TO_KERNEL_MAP(pdpt_clean_physical);
            if ( VMM_IS_PRESENT(pdpt_clean_kernel_map->pdts[j]) == false )
            {
                PDT_t* pdt_physical = VMM_TRANSLATE_KERNEL_MAP_TO_PHYSICAL(slab_allocator_allocate(slab_allocator));
                pdpt_clean_kernel_map->pdts[j] = (PDT_t*)(((qword)pdt_physical) | VMM_FLAGS_USER_TABLE);
            }

            byte* pdt_clean_physical = (byte*)(((qword)pdpt_clean_kernel_map->pdts[j]) & ~VMM_FLAGS_USER_TABLE);
            PDT_t* pdt_clean_kernel_map = VMM_TRANSLATE_PHYSICAL_TO_KERNEL_MAP(pdt_clean_physical);
            for ( size_t k = 0; k < VMM_ENTRIES_COUNT_IN_LEVEL; ++k )
            {
                if ( VMM_IS_PRESENT(pdt_clean_kernel_map->frames[k]) )
                {
                    continue;
                }

                pdt_clean_kernel_map->frames[k] = (void*)(((qword)frame) | VMM_FLAGS_USER_PAGE);
                return;
            }
        }
    }

    console_output_print_blue_screen("Failed to bind frame to page table.");
    while (1)
    {
        __asm__("hlt");
    }
}

void vmm_page_allocate(void* _pml4t)
{
    void* frame = pmm_frame_alloc();

    if ( frame == NULL )
    {
        console_output_print_blue_screen("Failed to allocate physical frame.");
        while (1)
        {
            __asm__("hlt");
        }
    }

    vmm_page_bind_to_frame(_pml4t, frame);
}

void vmm_page_free(void* _pml4t)
{
    PML4T_t* pml4t = _pml4t;
    console_output_print_blue_screen("vmm_page_free is not implemented");
    while (1)
    {
        __asm__("hlt");
    }
}
