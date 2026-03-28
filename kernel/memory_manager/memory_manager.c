#include <types.h>
#include <memory_manager.h>
#include <console_output.h>

#define MEMORY_MANAGER_MAP_BASE 0x00000500
#define MEMORY_MANAGER_FRAME_SIZE 0x200000

typedef struct
{
    qword base_address;
    qword length;
    dword type;
    dword extended_attributes;
} __attribute__((packed)) memory_manager_map_entry_t;

typedef struct
{
    dword entries_count;
    memory_manager_map_entry_t entries[];
} __attribute__((packed)) memory_manager_map_t;

typedef enum
{
    MEMORY_MANAGER_MAP_USABLE = 1,
    MEMORY_MANAGER_MAP_RESERVED = 2,
    MEMORY_MANAGER_MAP_ACPI_RECLAIMABLE = 3,
    MEMORY_MANAGER_MAP_ACPI_NVS = 4,
    MEMORY_MANAGER_MAP_BAD_MEMORY = 5
} memory_manager_map_entry_type_t;

static memory_manager_map_t* map = (memory_manager_map_t*) MEMORY_MANAGER_MAP_BASE;
static size_t current_entry = 0;

static inline void sanitize_memory_map(void);

static inline void sanitize_memory_map(void)
{
    for ( size_t i = 0; i < map->entries_count; ++i )
    {
        memory_manager_map_entry_t* entry_i = &map->entries[i];
        for ( size_t j = 0; j < map->entries_count; ++j )
        {
            if ( i == j )
            {
                continue;
            }

            memory_manager_map_entry_t* entry_j = &map->entries[j];
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
}

void memory_manager_setup(void)
{
    sanitize_memory_map();

    console_output_printf("Memory Map:\nEntries count: %lu\n", map->entries_count);
    for ( uint32_t i = 0; i < map->entries_count; ++i )
    {
        if ( map->entries[i].type != MEMORY_MANAGER_MAP_USABLE )
        {
            continue;
        }

        console_output_printf("     * Base address: %llx\n"
                              "     * Length: %llx\n"
                              "     * Type: %llx\n"
                              "     * Extended attributes: %llx\n",
                              map->entries[i].base_address,
                              map->entries[i].length,
                              map->entries[i].type,
                              map->entries[i].extended_attributes);

        break;
    }
}

void* memory_manager_frame_alloc(void)
{
    for ( ; current_entry < map->entries_count; ++current_entry )
    {
        memory_manager_map_entry_t* entry = &map->entries[current_entry];
        if ( entry->type == MEMORY_MANAGER_MAP_USABLE )
        {
            if ( entry->length >= MEMORY_MANAGER_FRAME_SIZE )
            {
                entry->base_address += MEMORY_MANAGER_FRAME_SIZE;
                entry->length -= MEMORY_MANAGER_FRAME_SIZE;
                return (void*) entry->base_address;
            }
        }
    }

    return NULL;
}

//void memory_manager_frame_free(void* ptr);
