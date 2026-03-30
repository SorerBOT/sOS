__asm__(".code32\n");

#include <stddef.h>
#include <stdbool.h>

#include <libc_partials/include/types.h>
#include <libc_partials/include/string.h>

#include <console_output.h>

#define KiB (1024)
#define MiB (1024 * (KiB))
#define PAGE_SIZE (2 * (MiB))
#define PAGE_TABLE_ENTRY_SIZE (4 * (KiB))
#define FLAGS 0b11
#define ENTRIES_IN_LEVEL 512

#define MEMORY_SIZE_TO_MAP 0x80000000
#define BASE_PHYSICAL_ADDRESS 0x000000
#define BASE_HIGHER_HALF_ADDRESS 0xFFFF800000000000
#define PAGES_TO_CREATE_COUNT ((MEMORY_SIZE_TO_MAP) / (PAGE_SIZE))

#define PML4T_HIGHER_HALF_OFFSET 256

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
    next_physical_address += PAGE_TABLE_ENTRY_SIZE;
    return temp;
}


PML4T_t* PML4T_init_identity_map()
{
    PML4T_t* pml4t = (PML4T_t*)(uint32_t) get_next_free_address();
    size_t created_pages_count = 0;
    for ( size_t i = 0; created_pages_count < PAGES_TO_CREATE_COUNT; ++i )
    {
        pml4t->pdpts[i] = get_next_free_address();
        PDPT_t* pdpt = (PDPT_t*)(uint32_t)pml4t->pdpts[i];

        for ( size_t j = 0; created_pages_count < PAGES_TO_CREATE_COUNT; ++j )
        {
            pdpt->pdts[j] = get_next_free_address();
            PDT_t* pdt = (PDT_t*)(uint32_t)pdpt->pdts[j];

            for ( size_t k = 0; k < ENTRIES_IN_LEVEL && created_pages_count < PAGES_TO_CREATE_COUNT; ++k, ++created_pages_count )
            {
                pdt->frames[k] = get_next_physical_address();
            }
        }
    }

    return pml4t;
}

void PML4T_init_higher_half_kernel(PML4T_t* pml4t)
{
    size_t created_pages_count = 0;
    for ( size_t i = PML4T_HIGHER_HALF_OFFSET; created_pages_count < PAGES_TO_CREATE_COUNT; ++i )
    {
        pml4t->pdpts[i] = get_next_free_address();
        PDPT_t* pdpt = (PDPT_t*)(uint32_t)pml4t->pdpts[i];

        for ( size_t j = 0; created_pages_count < PAGES_TO_CREATE_COUNT; ++j )
        {
            pdpt->pdts[j] = get_next_free_address();
            PDT_t* pdt = (PDT_t*)(uint32_t)pdpt->pdts[j];

            for ( size_t k = 0; k < ENTRIES_IN_LEVEL && created_pages_count < PAGES_TO_CREATE_COUNT; ++k, ++created_pages_count )
            {
                PDPT_t* lower_half_pdpt = (PDPT_t*)(uint32_t)pml4t->pdpts[i - PML4T_HIGHER_HALF_OFFSET];
                PDT_t* lower_half_pdt = (PDT_t*)(uint32_t)lower_half_pdpt->pdts[j];
                pdt->frames[k] = lower_half_pdt->frames[k];
            }
        }
    }
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

    PML4T_t* pml4t = PML4T_init_identity_map();
    PML4T_init_higher_half_kernel(pml4t);

    console_output_report("successfully created the kernel's page table.", CONSOLE_OUTPUT_SUCCESS);
}
