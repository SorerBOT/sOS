#include <types.h>
#include <memory_manager.h>
#include <console_output.h>

#define MEMORY_MANAGER_MAP_BASE 0x00000500

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

void memory_manager_setup(void)
{
    memory_manager_map_t* map = (memory_manager_map_t*) MEMORY_MANAGER_MAP_BASE;
    console_output_printf("Memory Map:\nEntries count: %lu\n", map->entries_count);
    for ( uint32_t i = 0; i < map->entries_count; ++i )
    {
        console_output_printf("     * Base address: %llx\n"
                              "     * Length: %llx\n"
                              "     * Type: %llx\n"
                              "     * Extended attributes: %llx\n",
                              map->entries[i].base_address,
                              map->entries[i].length,
                              map->entries[i].type,
                              map->entries[i].extended_attributes);
    }
}

void memory_manager_frame_alloc(void)
{

}

//void memory_manager_frame_free(void* ptr);
