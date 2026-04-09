__asm__(".code32\n");

#include <stddef.h>
#include <stdbool.h>

#include <libc_partials/include/types.h>
#include <libc_partials/include/string.h>

#include <console_output.h>

#define KiB (1024ULL)
#define MiB (1024ULL * (KiB))
#define GiB (1024ULL * (MiB))
#define PAGE_SIZE (2ULL * (MiB))
#define PAGE_TABLE_ENTRY_SIZE (4ULL * (KiB))
#define PAGE_TABLE_ENTRY_FLAGS 0b11
#define FRAME_FLAGS 0b10000011
#define ENTRIES_IN_LEVEL 512

#define MEMORY_SIZE_TO_MAP (16ULL * (GiB))
#define BASE_PHYSICAL_ADDRESS 0x000000ULL
//#define BASE_HIGHER_HALF_ADDRESS 0xFFFF800000000000
#define PAGES_TO_CREATE_COUNT ((MEMORY_SIZE_TO_MAP) / (PAGE_SIZE))
#define PML4T_HIGHER_HALF_OFFSET 256

#define MAKE_PAGE_TABLE_ENTRY(entry) (((uint32_t)entry) | PAGE_TABLE_ENTRY_FLAGS)
#define STRIP_PAGE_TABLE_ENTRY(entry_with_flags) ((void*)(((uint32_t)entry_with_flags) & ~PAGE_TABLE_ENTRY_FLAGS))
#define MAKE_FRAME(frame) (((uint32_t)frame) | FRAME_FLAGS)
#define STRIP_FRAME(frame_with_flags) ((void*)(((uint32_t)frame_with_flags) & ~FRAME_FLAGS))

typedef uint64_t lm_pointer;

typedef struct
{
    lm_pointer frames[ENTRIES_IN_LEVEL];
} PDT_t;

typedef struct
{
    lm_pointer pdts[ENTRIES_IN_LEVEL];
} PDPT_t;

typedef struct
{
    lm_pointer pdpts[ENTRIES_IN_LEVEL];
} PML4T_t;


extern dword BASE_PAGE_TABLE_ADDRESS;
lm_pointer next_free_address = 0xDEADBEEF;
lm_pointer next_physical_address = BASE_PHYSICAL_ADDRESS;

void next_free_address_init()
{
    next_free_address = BASE_PAGE_TABLE_ADDRESS;
}

lm_pointer get_next_free_address()
{
    memset((void*)(uint32_t)next_free_address, 0, PAGE_TABLE_ENTRY_SIZE);
    lm_pointer temp = next_free_address;
    next_free_address += PAGE_TABLE_ENTRY_SIZE;
    return temp;
}

lm_pointer get_next_physical_address()
{
    lm_pointer temp = next_physical_address;
    next_physical_address += PAGE_SIZE;
    return temp;
}


/*
 * Inits the:
 *  1. identity map
 *  2. kernel map (of the entire memory)
 *  3. kernel binary page
 */
PML4T_t* PML4T_init_map()
{
    PML4T_t* pml4t = (PML4T_t*)(uint32_t) get_next_free_address();
    memset(pml4t, 0, sizeof(PML4T_t));

    /*
     * Setting up the identity map
     */
    size_t created_pages_count = 0;
    for ( size_t i = 0; i < ENTRIES_IN_LEVEL && created_pages_count < PAGES_TO_CREATE_COUNT; ++i )
    {
        pml4t->pdpts[i] = MAKE_PAGE_TABLE_ENTRY(get_next_free_address());
        PDPT_t* pdpt = STRIP_PAGE_TABLE_ENTRY(pml4t->pdpts[i]);

        for ( size_t j = 0; j < ENTRIES_IN_LEVEL && created_pages_count < PAGES_TO_CREATE_COUNT; ++j )
        {
            pdpt->pdts[j] = MAKE_PAGE_TABLE_ENTRY(get_next_free_address());
            PDT_t* pdt = STRIP_PAGE_TABLE_ENTRY(pdpt->pdts[j]);

            for ( size_t k = 0; k < ENTRIES_IN_LEVEL && created_pages_count < PAGES_TO_CREATE_COUNT; ++k, ++created_pages_count )
            {
                pdt->frames[k] = MAKE_FRAME(get_next_physical_address());
            }
        }
    }

    /*
     * Setting up the kernel map
     */
    for ( size_t i = 0; i + PML4T_HIGHER_HALF_OFFSET < ENTRIES_IN_LEVEL; ++i )
    {
        pml4t->pdpts[i + PML4T_HIGHER_HALF_OFFSET] = pml4t->pdpts[i];
    }

    /*
     * Setting up the kernel binary page 
     */
    if ( pml4t->pdpts[511] == 0x0000 )
    {
        pml4t->pdpts[511] = MAKE_PAGE_TABLE_ENTRY(get_next_free_address());
    }

    PDPT_t* kernel_binary_pdpt = STRIP_PAGE_TABLE_ENTRY(pml4t->pdpts[511]);
    if ( kernel_binary_pdpt->pdts[510] == 0x0000 )
    {
        kernel_binary_pdpt->pdts[510] = MAKE_PAGE_TABLE_ENTRY(get_next_free_address());
    }
    PDT_t* kernel_binary_pdt = STRIP_PAGE_TABLE_ENTRY(kernel_binary_pdpt->pdts[510]);
    kernel_binary_pdt->frames[0] = MAKE_FRAME(0x0000);

    return pml4t;
}

void page_table_setup()
{
    console_output_init_settings_t settings =
    {
        .initial_line = 21,
        .should_copy_existing_buffer = true
    };
    console_output_init(&settings);
    console_output_report("entered 32-bit protected mode.", CONSOLE_OUTPUT_SUCCESS);

    next_free_address_init();

    PML4T_t* pml4t = PML4T_init_map();

    console_output_report("successfully created the kernel's page table.", CONSOLE_OUTPUT_SUCCESS);
}


