#include <ext2.h>
#include <types.h>
#include <console_output.h>
#include <filesystem.h>
#include <disk_block_io.h>
#include <string.h>
#include <kernel_allocator.h>

#define BOOT_BLOCK_SIZE_IN_BYTES 1024
#define SUPER_BLOCK_MAGIC_NUMBER 0xEF53

#define EXT2_FILESYSTEM_STATE_CLEAN 1
#define EXT2_FILESYSTEM_STATE_ERROR 2

#define EXT2_INODE_TYPE_MASK                    0xF000
#define EXT2_INODE_TYPE_FIFO                    0x1000
#define EXT2_INODE_TYPE_CHARACTER_DEVICE        0x2000
#define EXT2_INODE_TYPE_DIRECTORY               0x4000
#define EXT2_INODE_TYPE_BLOCK_DEVICE            0x6000
#define EXT2_INODE_TYPE_REGULAR_FILE            0x8000
#define EXT2_INODE_TYPE_SYMBOLIC_LINK           0xA000
#define EXT2_INODE_TYPE_UNIX_SOCKET             0xC000

#define EXT2_INODE_PERMISSIONS_MASK             0x0FFF
#define EXT2_INODE_PERMISSIONS_OTHER_EXECUTE    0x0001
#define EXT2_INODE_PERMISSIONS_OTHER_WRITE      0x0002
#define EXT2_INODE_PERMISSIONS_OTHER_READ       0x0004
#define EXT2_INODE_PERMISSIONS_GROUP_EXECUTE    0x0008
#define EXT2_INODE_PERMISSIONS_GROUP_WRITE      0x0010
#define EXT2_INODE_PERMISSIONS_GROUP_READ       0x0020
#define EXT2_INODE_PERMISSIONS_USER_EXECUTE     0x0040
#define EXT2_INODE_PERMISSIONS_USER_WRITE       0x0080
#define EXT2_INODE_PERMISSIONS_USER_READ        0x0100
#define EXT2_INODE_PERMISSIONS_STICKY_BIT       0x0200
#define EXT2_INODE_PERMISSIONS_SET_GROUP_ID     0x0400
#define EXT2_INODE_PERMISSIONS_SET_USER_ID      0x0800

typedef struct
{
    dword inodes_count;
    dword blocks_count;
    dword reserved_blocks_count;
    dword free_blocks_count;
    dword free_inodes_count;
    dword number_of_block_containing_super_block;
    dword log_block_size;
    dword log_frag_size;
    dword blocks_per_group_count;
    dword frags_per_group_count;
    dword inodes_per_group_count;
    dword last_mount_posix_time;
    dword last_write_posix_time;
    word mounts_since_fsck_count;
    word max_mounts_before_fsck_count;
    word magic_number;
    word state;
    word errors;
    word minor_revision_level;
    dword last_fsck_posix_time;
    dword max_posix_time_before_fsck_required;
    dword filesystem_creator_os;
    dword revision_level;
    word default_reserved_block_user_id;
    word default_reserved_block_group_id;

/*
 * EXTENDED SUPERBLOCK FIELDS (VALID IF revision_level >= 1)
 */

    dword first_non_reserved_inode_number;
    word size_of_inode;
    word block_group_of_super_block;
    dword compatible_features_bitmask;
    dword incompatible_features_bitmask;
    dword read_only_compatible_features_bitmask;
    byte volume_uuid[16];

    char volume_name[16];
    char name_of_last_mounted_directory[64];

    dword compression_algorithm_usage_bitmask;

    byte _padding[820];
} __attribute__((packed)) ext2_super_block_t;

typedef struct
{
    dword block_address_of_block_usage_bitmap;
    dword block_address_of_inode_usage_bitmap;
    dword block_address_of_inode_table;
    word unallocated_blocks_in_group_count;
    word unallocated_inodes_in_group_count;
    word directories_in_group_count;

    byte _padding[14];
} __attribute__((packed)) ext2_block_group_descriptor_t;

typedef struct
{
    word type_and_permissions;
    word user_id;
    dword size_in_bytes_lower;
    dword last_access_posix_time;
    dword creation_posix_time;
    dword last_modification_posix_time;
    dword deletion_posix_time;
    word group_id;
    word hardlinks_count;
    dword occupied_disk_sectors_count;
    dword flags;
    dword os_specific_value_1;
    dword direct_block_numbers[12];
    dword singly_indirect_block_number;
    dword doubly_indirect_block_number;
    dword triply_indirect_block_number;
    dword generation_number;
    dword version_specific_1;
    dword version_specific_2;
    dword block_address_of_fragment;
    byte os_specific_value_2[12];
} __attribute__((packed)) ext2_inode_t;

typedef struct
{
    dword inode_number;
    word size;
    byte name_length;
    byte file_type;
    char name[];
} __attribute__((packed)) ext2_directory_entry_t;

typedef struct
{
    ext2_block_group_descriptor_t* block_group_descriptors;
    dword block_group_inode_table_size;
    dword block_groups_count;
    dword block_size;
    ext2_super_block_t super_block;
} ext2_filesystem_metadata_t;

static ext2_filesystem_metadata_t filesystem_metadata;

static inline void validate_boot_block(const byte boot_block[BOOT_BLOCK_SIZE_IN_BYTES]);
static inline void validate_super_block(const ext2_super_block_t* super_block);
static inline void load_filesystem_metadata(ext2_filesystem_metadata_t* filesystem_metadata);
static inline dword get_block_size_from_super_block(const ext2_super_block_t* super_block);
static inline dword get_block_group_of_inode(dword inode_number, const ext2_filesystem_metadata_t* filesystem_metadata);
static inline dword get_block_group_of_block(dword block_number, const ext2_filesystem_metadata_t* filesystem_metadata);
static inline dword get_in_group_index_of_inode(dword inode_number, const ext2_filesystem_metadata_t* filesystem_metadata);
static inline dword get_in_group_index_of_block(dword block_number, const ext2_filesystem_metadata_t* filesystem_metadata);
static inline dword get_in_group_index_of_block_containing_inode(dword inode_number, const ext2_filesystem_metadata_t* filesystem_metadata);

static inline void validate_boot_block(const byte boot_block[BOOT_BLOCK_SIZE_IN_BYTES])
{
    for ( dword i = 0; i < 1024; ++i )
    {
        if ( boot_block[i] != 0 )
        {
            console_output_print_blue_screen("Ext2 Error: boot block contains non-zero opcodes.\n");
            while ( 1 )
            {
                __asm__ volatile("hlt");
            }
        }
    }
}

static inline void validate_super_block(const ext2_super_block_t* super_block)
{
    if ( super_block->magic_number != SUPER_BLOCK_MAGIC_NUMBER )
    {
        console_output_print_blue_screen("Ext2 Error: super block contains the wrong magic number.\n");
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    if ( super_block->log_block_size > 2 )
    {
        console_output_print_blue_screen("Ext2 Error: the log of block size value of %zu is invalid.\n", super_block->log_block_size);
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    dword block_size = get_block_size_from_super_block(super_block);
    if ( block_size != FILESYSTEM_BLOCK_SIZE_IN_BYTES )
    {
        console_output_print_blue_screen("Ext2 Error: block size of %zu is different from FILESYSTEM_BLOCK_SIZE_IN_BYTES = %zu.\n", block_size, FILESYSTEM_BLOCK_SIZE_IN_BYTES);
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    // APPARENTLY THIS IS OK, AS INODE SIZE VARIES
    // dword inode_size = super_block->size_of_inode;
    // if ( inode_size != sizeof(ext2_inode_t) )
    // {
    //     console_output_print_blue_screen("Ext2 Error: inode size of %zu is different from sizeof(ext2_inode_t) = %zu.\n", inode_size, sizeof(ext2_inode_t));
    //     while ( 1 )
    //     {
    //         __asm__ volatile("hlt");
    //     }
    // }

    dword block_groups_count_v1 = 1 + ((super_block->blocks_count - 1) / super_block->blocks_per_group_count);
    dword block_groups_count_v2 = super_block->inodes_count / super_block->inodes_per_group_count;
    if ( block_groups_count_v1 != block_groups_count_v2 )
    {
        console_output_print_blue_screen("Ext2 Error: the two methods of inferring the number of block groups yielded different results.\n");
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    if ( super_block->state == EXT2_FILESYSTEM_STATE_ERROR )
    {
        console_output_print_blue_screen("Ext2 Error: filesystem is in error state.\n");
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }
    else if ( super_block->state != EXT2_FILESYSTEM_STATE_CLEAN )
    {
        console_output_print_blue_screen("Ext2 Error: filesystem state is out of range.\n");
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }
}

static inline void load_filesystem_metadata(ext2_filesystem_metadata_t* filesystem_metadata)
{
    /*
     * Loading Boot Block
     */
    byte first_block[EXT2_BLOCK_SIZE_IN_BYTES];
    disk_block_io_read_block(0, first_block, EXT2_BLOCK_SIZE_IN_BYTES);


    byte boot_block[BOOT_BLOCK_SIZE_IN_BYTES];
    memcpy(boot_block, first_block, BOOT_BLOCK_SIZE_IN_BYTES);
    validate_boot_block(boot_block);

    ext2_super_block_t super_block;
    dword block_groups_count;
    dword block_size;
    dword block_group_inode_table_size;
    ext2_block_group_descriptor_t* block_group_descriptors;

    /*
     * Loading Super Block
     */
    memcpy(&super_block, first_block + sizeof(boot_block), sizeof(super_block));
    validate_super_block(&super_block);

    /*
     * Loading General Metadata
     */
    block_groups_count = super_block.inodes_count / super_block.inodes_per_group_count;
    block_size = get_block_size_from_super_block(&super_block);
    block_group_inode_table_size = super_block.inodes_per_group_count * super_block.size_of_inode;
    block_group_descriptors = kernel_allocator_allocate(block_groups_count * sizeof(*block_group_descriptors));
    if ( block_group_descriptors == NULL )
    {
        console_output_print_blue_screen("Ext2 Error: failed to allocate memory.\n");
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }
    disk_block_io_read_blocks_from(1, (byte*) block_group_descriptors, block_groups_count * sizeof(*block_group_descriptors));

    *filesystem_metadata = (ext2_filesystem_metadata_t)
    {
        .super_block = super_block,
        .block_groups_count = block_groups_count,
        .block_size = block_size,
        .block_group_inode_table_size = block_group_inode_table_size,
        .block_group_descriptors = block_group_descriptors
    };
}

static inline dword get_block_size_from_super_block(const ext2_super_block_t* super_block)
{
    return 1024U << super_block->log_block_size;
}

static inline dword get_block_group_of_inode(dword inode_number, const ext2_filesystem_metadata_t* filesystem_metadata)
{
    return (inode_number - 1) / filesystem_metadata->super_block.inodes_per_group_count;
}

static inline dword get_block_group_of_block(dword block_number, const ext2_filesystem_metadata_t* filesystem_metadata)
{

    return block_number / filesystem_metadata->super_block.blocks_per_group_count;
}

static inline dword get_in_group_index_of_inode(dword inode_number, const ext2_filesystem_metadata_t* filesystem_metadata)
{
    return (inode_number - 1) % filesystem_metadata->super_block.inodes_per_group_count;
}

static inline dword get_in_group_index_of_block(dword block_number, const ext2_filesystem_metadata_t* filesystem_metadata)
{
    return block_number % filesystem_metadata->super_block.blocks_per_group_count;
}

static inline dword get_in_group_index_of_block_containing_inode(dword inode_number, const ext2_filesystem_metadata_t* filesystem_metadata)
{
    dword in_group_index_of_inode = get_in_group_index_of_inode(inode_number, filesystem_metadata);
    return (in_group_index_of_inode * filesystem_metadata->super_block.size_of_inode) / FILESYSTEM_BLOCK_SIZE_IN_BYTES;
}

void ext2_setup(void)
{
    load_filesystem_metadata(&filesystem_metadata);
    console_output_report("Detected a valid ext2 filesystem.\n", CONSOLE_OUTPUT_SUCCESS);
}

void ext2_print_directory_contents(void)
{
    dword root_directory_inode_number = 2;
    dword block_group_of_root_directory_inode = get_block_group_of_inode(root_directory_inode_number, &filesystem_metadata);
    const ext2_block_group_descriptor_t* block_group_descriptor = &filesystem_metadata.block_group_descriptors[block_group_of_root_directory_inode];
    dword in_group_inode_index = get_in_group_index_of_inode(root_directory_inode_number, &filesystem_metadata);
    console_output_printf("In Group inode index: %zu\n", in_group_inode_index); // prints as 1 as expected

    dword block_group_inode_table_base_block = block_group_descriptor->block_address_of_inode_table;
    ext2_inode_t* block_group_inode_table = kernel_allocator_allocate(filesystem_metadata.block_group_inode_table_size);
    if ( block_group_inode_table == NULL )
    {
        console_output_print_blue_screen("Ext2 Error: failed to allocate memory.\n");
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    disk_block_io_read_blocks_from(
            block_group_inode_table_base_block,
            (byte*) block_group_inode_table,
            filesystem_metadata.block_group_inode_table_size);
  
    ext2_inode_t* inode_entry = (ext2_inode_t*)(
            ((byte*) block_group_inode_table) + (filesystem_metadata.super_block.size_of_inode * in_group_inode_index)
            );

    console_output_printf("Type and permissions: %zu\n", inode_entry->type_and_permissions); // prints 0


    dword first_data_block_of_inode = inode_entry->direct_block_numbers[0];
    console_output_printf("First Datablock of inode: %zu\n", first_data_block_of_inode); // prints 0 for some reason
    ext2_directory_entry_t* directory_entries = kernel_allocator_allocate(FILESYSTEM_BLOCK_SIZE_IN_BYTES);
    if ( directory_entries == NULL )
    {
        console_output_print_blue_screen("Ext2 Error: failed to allocate memory.\n");
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    disk_block_io_read_block(first_data_block_of_inode, (byte*) directory_entries, FILESYSTEM_BLOCK_SIZE_IN_BYTES);

    for ( const ext2_directory_entry_t* entry = &directory_entries[0]; (byte*)entry < ((byte*)directory_entries) + FILESYSTEM_BLOCK_SIZE_IN_BYTES; )
    {
        console_output_printf("-------------------------------------------------------------\n");
        if ( entry->size == 0 )
        {
            console_output_printf("entry size is 0\n");
            break;
        }

        if ( entry->inode_number != 0 && entry->name_length > 0 )
        {
            console_output_printf("inode number: %zu\n", entry->inode_number);
            console_output_printf("name: ");
            for ( size_t i = 0; i < entry->name_length; ++i )
            {
                console_output_printf("%c", entry->name[i]);
            }
            console_output_printf("\n");
        }

        entry = (const ext2_directory_entry_t*)(((byte*) entry) + entry->size);
    }
    console_output_printf("-------------------------------------------------------------\n");

    kernel_allocator_free(block_group_inode_table);
    kernel_allocator_free(directory_entries);
}
