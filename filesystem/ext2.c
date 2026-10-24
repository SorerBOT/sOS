#include <ext2.h>
#include <types.h>
#include <console_output.h>
#include <filesystem.h>
#include <disk_block_io.h>
#include <string.h>

#define BOOT_BLOCK_SIZE_IN_BYTES 1024
#define SUPER_BLOCK_SIZE_IN_BYTES 1024

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

static byte super_block[SUPER_BLOCK_SIZE_IN_BYTES];

static inline void validate_boot_block(const byte boot_block[BOOT_BLOCK_SIZE_IN_BYTES]);
static inline void load_superblock(void);


static inline void validate_boot_block(const byte boot_block[BOOT_BLOCK_SIZE_IN_BYTES])
{
    for ( size_t i = 0; i < 1024; ++i )
    {
        if ( boot_block[i] != 0 )
        {
            console_output_print_blue_screen("Filesystem Error: boot block contains non-zero opcodes.\n");
            while ( 1 )
            {
                __asm__ volatile("hlt");
            }
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


    memcpy(super_block, first_block + sizeof(boot_block), sizeof(super_block));
}


void ext2_setup(void)
{
    load_superblock();
    console_output_printf("Super Block: %1024s\n", super_block);
}
