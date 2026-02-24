__asm__(".code32\n");

#include <stddef.h>
#include <stdbool.h>
#include <types.h>
#include <string.h>

//#include <console_io.h>

#define KiB (1024)
#define PAGE_SIZE (4 * KiB)
#define FLAGS 0b11

#define MEMORY_SIZE_TO_MAP 0x40000000
#define BASE_PHYSICAL_ADDRESS 0x000000

#define PAGES_IN_PT 512
#define PAGES_IN_PDT (PAGES_IN_PT * 512)
#define PAGES_IN_PDPT (PAGES_IN_PDT * 512)

typedef uint64_t lm_pointer;

typedef struct
{
    byte data[PAGE_SIZE];
} Frame_t;

typedef struct
{
    Frame_t* mapped_frame_physical_address;
} Page_t;

typedef struct
{
    lm_pointer pages[PAGE_SIZE / sizeof(lm_pointer)];
} PT_t;

typedef struct
{
    lm_pointer pts[PAGE_SIZE / sizeof(lm_pointer)];
} PDT_t;

typedef struct
{
    lm_pointer pdts[PAGE_SIZE / sizeof(lm_pointer)];
} PDPT_t;

typedef struct
{
    lm_pointer pdpts[PAGE_SIZE / sizeof(lm_pointer)];
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
    memset((void*)(uint32_t)next_free_address, 0, PAGE_SIZE);
    lm_pointer temp = next_free_address;
    next_free_address += PAGE_SIZE;
    return temp;
}

lm_pointer get_next_physical_address()
{
    lm_pointer temp = next_physical_address;
    next_physical_address += PAGE_SIZE;
    return temp;
}

void PT_init_identity_map(PT_t* pt, int64_t pages_count)
{
    size_t created_pages_count = 0;
    for (; pages_count > 0 && created_pages_count < 512; --pages_count, ++created_pages_count)
    {
        pt->pages[created_pages_count] = (get_next_physical_address() | FLAGS);
    }
}

void PDT_init_identity_map(PDT_t* pdt, int64_t pages_count)
{
    //console_io_printf("Mapping pages. %ld pages remaining...\n", pages_count);


    size_t created_pt_count = 0;
    for (; pages_count > 0 && created_pt_count < 512; pages_count -= PAGES_IN_PT, ++created_pt_count)
    {
        pdt->pts[created_pt_count] = get_next_free_address();
        PT_init_identity_map((PT_t*)(uint32_t)pdt->pts[created_pt_count], pages_count);
        pdt->pts[created_pt_count] |= FLAGS;
    }
}

void PDPT_init_identity_map(PDPT_t* pdpt, int64_t pages_count)
{
    size_t created_pdt_count = 0;
    for (; pages_count > 0 && created_pdt_count < 512; pages_count -= PAGES_IN_PDT, ++created_pdt_count)
    {
        pdpt->pdts[created_pdt_count] = get_next_free_address();
        PDT_init_identity_map((PDT_t*)(uint32_t)pdpt->pdts[created_pdt_count], pages_count);
        pdpt->pdts[created_pdt_count] |= FLAGS;
    }
}

PML4T_t* PML4T_init_identity_map(size_t memory_size_to_map)
{
    int64_t pages_count = memory_size_to_map / PAGE_SIZE;
    //console_io_printf("Mapping %ld pages of memory...\n", pages_count);

    PML4T_t* pml4t = (PML4T_t*)(uint32_t) get_next_free_address();

    size_t created_pdpt_count = 0;
    for (; pages_count > 0 && created_pdpt_count < 512; pages_count -= PAGES_IN_PDPT, ++created_pdpt_count)
    {
        pml4t->pdpts[created_pdpt_count] = get_next_free_address();
        PDPT_init_identity_map((PDPT_t*)(uint32_t)pml4t->pdpts[created_pdpt_count], pages_count);
        pml4t->pdpts[created_pdpt_count] |= FLAGS;
    }

    return pml4t;
}

void page_table_setup()
{
    //console_io_init_settings_t settings =
    //{
    //    .initial_line = 13,
    //    .should_copy_existing_buffer = true
    //};

    //console_io_init(&settings);

    //console_io_report("stage 2 completed...", CONSOLE_IO_SUCCESS);
    //console_io_report("entered 32-bit protected mode...", CONSOLE_IO_SUCCESS);
    next_free_address_init();

    PML4T_t* pml4t = PML4T_init_identity_map(MEMORY_SIZE_TO_MAP);

    //console_io_report("successfully created the kernel's page table...", CONSOLE_IO_SUCCESS);
}
