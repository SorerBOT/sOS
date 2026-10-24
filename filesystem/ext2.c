#include <ext2.h>
#include <types.h>
#include <console_output.h>
#include <filesystem.h>
#include <disk_block_io.h>
#include <string.h>

#define BOOT_BLOCK_SIZE_IN_BYTES 1024
#define SUPER_BLOCK_MAGIC_NUMBER 0xEF53

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
    word default_reserved_block_uid;
    word default_reserved_block_gid;

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

static ext2_super_block_t super_block;
static size_t block_groups_count;

static inline void validate_super_block(const ext2_super_block_t* super_block);
static inline void validate_boot_block(const byte boot_block[BOOT_BLOCK_SIZE_IN_BYTES]);
static inline void load_superblock(void);


static inline void validate_boot_block(const byte boot_block[BOOT_BLOCK_SIZE_IN_BYTES])
{
    for ( size_t i = 0; i < 1024; ++i )
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

    size_t block_size = 1024 << super_block->log_block_size;
    if ( block_size != FILESYSTEM_BLOCK_SIZE_IN_BYTES )
    {
        console_output_print_blue_screen("Ext2 Error: block size of %zu is different from FILESYSTEM_BLOCK_SIZE_IN_BYTES = %zu.\n", block_size, FILESYSTEM_BLOCK_SIZE_IN_BYTES);
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    size_t block_groups_count_v1 = super_block->blocks_count / super_block->blocks_per_group_count;
    size_t block_groups_count_v2 = super_block->inodes_count / super_block->inodes_per_group_count;

    if ( block_groups_count_v1 != block_groups_count_v2 )
    {
        console_output_print_blue_screen("Ext2 Error: the two methods of inferring the number of block groups yielded different results.\n");
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

}

static inline void load_superblock(void)
{
    byte first_block[EXT2_BLOCK_SIZE_IN_BYTES];
    disk_block_io_read_block(0, first_block, EXT2_BLOCK_SIZE_IN_BYTES);


    byte boot_block[BOOT_BLOCK_SIZE_IN_BYTES];
    memcpy(boot_block, first_block, BOOT_BLOCK_SIZE_IN_BYTES);
    validate_boot_block(boot_block);

    memcpy(&super_block, first_block + sizeof(boot_block), sizeof(super_block));
    validate_super_block(&super_block);
}


void ext2_setup(void)
{
    load_superblock();
    console_output_report("Detected a valid ext2 filesystem.\n", CONSOLE_OUTPUT_SUCCESS);
}
