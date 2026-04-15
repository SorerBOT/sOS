#include <types.h>
#include <pmm.h>
#include <console_output.h>
#include <vmm.h>
#include <interval.h>

typedef struct
{
    qword base_address;
    qword length;
    dword type;
    dword extended_attributes;
} __attribute__((packed)) pmm_map_entry_t;

typedef struct
{
    pmm_map_entry_t* entries;
    size_t entries_count;
} __attribute__((packed)) pmm_map_t;

typedef enum
{
    PMM_MAP_USABLE = 1,
    PMM_MAP_RESERVED = 2,
    PMM_MAP_ACPI_RECLAIMABLE = 3,
    PMM_MAP_ACPI_NVS = 4,
    PMM_MAP_BAD_MEMORY = 5
} pmm_map_entry_type_t;

typedef struct
{
    void* base_address;
} pmm_frame_t;

typedef struct
{
    size_t frames_count;
    pmm_frame_t frames[];
} __attribute__((packed)) pmm_allocator_stack_t;

typedef struct
{
    size_t frames_count;
    bool frames_is_allocated[];
} __attribute__((packed)) pmm_allocator_bitmap_t;

static pmm_allocator_bitmap_t* allocator_bitmap = NULL;
static pmm_allocator_stack_t* allocator_stack = NULL;

extern char kernel_base_physical_address;
extern char kernel_end_physical_address;

static inline void sanitize_memory_map(pmm_map_t map);
static inline void* get_next_frame(pmm_map_t map);
static inline void* get_highest_address(pmm_map_t map);
static inline size_t get_frame_index(void* base_address);

static inline void sanitize_memory_map(pmm_map_t map)
{
    /*
     * Remove repeating and overlapping memory maps
     */
    for ( size_t i = 0; i < map.entries_count; ++i )
    {
        pmm_map_entry_t* entry_i = &map.entries[i];
        for ( size_t j = 0; j < map.entries_count; ++j )
        {
            if ( i == j )
            {
                continue;
            }

            pmm_map_entry_t* entry_j = &map.entries[j];
            if ( entry_i->base_address <= entry_j->base_address && entry_i->length >= entry_j->base_address )
            {
                if ( entry_i->type >= entry_j->type  )
                {
                    entry_j->base_address =  entry_i->base_address + entry_i->length;
                    entry_j->length -= entry_i->length - ( entry_j->base_address - entry_i->base_address );
                }
                else
                {
                    entry_i->length = entry_j->base_address - entry_i->base_address;
                }
            }
        }
    }

    /*
     * Make sure frames are PMM_FRAME_SIZE (2MiB) aligned
     */
    for ( size_t i = 0; i < map.entries_count; ++i )
    {
        pmm_map_entry_t* entry = &map.entries[i];

        qword remainder = entry->base_address & (PMM_FRAME_SIZE - 1);
        if ( remainder > 0 )
        {
            qword bytes_to_align = PMM_FRAME_SIZE - remainder;
            if ( entry->length >= bytes_to_align )
            {
                entry->base_address += bytes_to_align;
                entry->length -= bytes_to_align;
            }
        }
    }
}


static inline void* get_next_frame(pmm_map_t map)
{
    static size_t current_entry = 0;

    interval_t kernel_binary_addresses =
    {
        .start = (size_t) &kernel_base_physical_address,
        .end = ((size_t) &kernel_end_physical_address) - 1
    };

    for ( ; current_entry < map.entries_count; ++current_entry )
    {
        pmm_map_entry_t* entry = &map.entries[current_entry];
        if ( entry->type != PMM_MAP_USABLE )
        {
            continue;
        }

        for ( ; entry->length >= PMM_FRAME_SIZE; entry->base_address += PMM_FRAME_SIZE, entry->length -= PMM_FRAME_SIZE )
        {
            interval_t current_frame_addresses =
            {
                .start = entry->base_address,
                .end = entry->base_address + PMM_FRAME_SIZE - 1
            };

            if ( interval_closed_is_intersecting(kernel_binary_addresses, current_frame_addresses) )
            {
                continue;
            }

            entry->base_address += PMM_FRAME_SIZE;
            entry->length -= PMM_FRAME_SIZE;

            qword frame_address = entry->base_address;
            return (void*) frame_address;
        }
    }

    return NULL;
}

static inline void* get_highest_address(pmm_map_t map)
{
    qword highest_address = 0;

    for ( size_t i = 0; i < map.entries_count; ++i )
    {
        if ( map.entries[i].type != PMM_MAP_USABLE )
        {
            continue;
        }

        qword current_address = map.entries[i].base_address + map.entries[i].length;
        if ( current_address > highest_address + 1 )
        {
            highest_address = current_address - 1;
        }
    }

    return (void*) highest_address;
}

static inline size_t get_frame_index(void* base_address)
{
    if ( ((qword)base_address) & (PMM_FRAME_SIZE - 1) )
    {
        console_output_print_blue_screen("Unaligned physical frame at address: %p\n", base_address);
        while (1)
        {
            __asm__("hlt");
        }
        return 0; // didn't want to use __builtin_unreachable.
    }

    else
    {
        return ((qword)base_address) / PMM_FRAME_SIZE;
    }
}

void pmm_setup(qword memory_map_address, word memory_map_entries_count)
{
    pmm_map_t map =
    {
        .entries_count = memory_map_entries_count,
        .entries = VMM_TRANSLATE_PHYSICAL_TO_KERNEL_MAP(memory_map_address)
    };

    sanitize_memory_map(map);

    void* highest_address = get_highest_address(map);
    void* current_address = 0;

    allocator_bitmap = VMM_TRANSLATE_PHYSICAL_TO_KERNEL_MAP(get_next_frame(map));
    if ( allocator_bitmap == NULL )
    {
        console_output_print_blue_screen("Failed to allocate memory for memory manager.\n");
        while (1)
        {
            __asm__("hlt");
        }
    }
    allocator_bitmap->frames_count = 0;

    for ( ; (qword)current_address < (qword)highest_address; current_address += PMM_FRAME_SIZE )
    {
        allocator_bitmap->frames_is_allocated[allocator_bitmap->frames_count++] = true;
    }

    allocator_stack = (pmm_allocator_stack_t*) (((byte*)allocator_bitmap) + sizeof(*allocator_bitmap) + allocator_bitmap->frames_count * sizeof(*allocator_bitmap->frames_is_allocated));
    allocator_stack->frames_count = 0;
    for ( ;; )
    {
        qword allocator_frame_offset = (qword)(((byte*)&allocator_stack->frames[allocator_stack->frames_count]) - ((byte*)allocator_bitmap));
        if ( allocator_frame_offset + sizeof(*allocator_stack->frames)  >= PMM_FRAME_SIZE )
        {
            break;
        }

        void* current_frame_address = get_next_frame(map);

        if ( current_frame_address == NULL )
        {
            break;
        }

        else
        {
            allocator_stack->frames[allocator_stack->frames_count++] = (pmm_frame_t)
            {
                .base_address = current_frame_address
            };

            size_t frame_index = get_frame_index(current_frame_address);
            if ( frame_index > allocator_bitmap->frames_count )
            {
                console_output_print_blue_screen("Frame index: %llu is greater exceeds existing frames\n", frame_index);
                while (1)
                {
                    __asm__("hlt");
                }
                break;
            }

            allocator_bitmap->frames_is_allocated[frame_index] = false;
        }
    }

    //console_output_printf("Memory Map:\nEntries count: %lu\n", map.entries_count);
    //for ( uint32_t i = 0; i < map.entries_count; ++i )
    //{
    //    if ( map.entries[i].type != PMM_MAP_USABLE )
    //    {
    //        continue;
    //    }

    //    console_output_printf("     * Base address: %llx\n"
    //                          "     * Length: %llx\n"
    //                          "     * Type: %llx\n"
    //                          "     * Extended attributes: %llx\n",
    //                          map.entries[i].base_address,
    //                          map.entries[i].length,
    //                          map.entries[i].type,
    //                          map.entries[i].extended_attributes);

    //}
}

void* pmm_frame_alloc(void)
{
    if ( allocator_stack->frames_count == 0 )
    {
        return NULL;
    }

    else
    {
        void* address = allocator_stack->frames[--allocator_stack->frames_count].base_address;
        size_t frame_index = get_frame_index(address);
        allocator_bitmap->frames_is_allocated[frame_index] = true;
        return address;
    }
}

void pmm_frame_free(void* ptr)
{
    size_t frame_index = get_frame_index(ptr);
    if ( allocator_bitmap->frames_is_allocated[frame_index] == true )
    {
        allocator_bitmap->frames_is_allocated[frame_index] = false;
        allocator_stack->frames[allocator_stack->frames_count++] = (pmm_frame_t)
        {
            .base_address = ptr
        };
    }

    else
    {
        console_output_print_blue_screen("Trying to free a frame that is not allocated with index: %llu.\n", frame_index);
        while (1)
        {
            __asm__("hlt");
        }
    }
}
