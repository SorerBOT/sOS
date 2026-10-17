#include <disk_block_io.h>
#include <disk_driver.h>
#include <filesystem.h>
#include <console_output.h>

#define DISK_BLOCK_IO_BLOCK_TO_FIRST_SECTOR(block) (((block) + (FILESYSTEM_FIRST_BLOCK_OFFSET)) * (FILESYSTEM_BLOCK_SIZE_IN_SECTORS)) 

static size_t blocks_count;

void disk_block_io_setup(void)
{
    blocks_count = disk_driver_get_sector_count() / FILESYSTEM_BLOCK_SIZE_IN_SECTORS;
}

void disk_block_io_read_block(size_t block_number, byte* dst, size_t dst_size)
{
    if ( block_number >= blocks_count )
    {
        console_output_print_blue_screen("Disk Block IO Error: provided block number of %llu is out of range for a disk with a block count of %llu\n", block_number, blocks_count);
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    if ( dst_size < FILESYSTEM_BLOCK_SIZE_IN_BYTES )
    {
        console_output_print_blue_screen("Disk Block IO Error: provided buffer of size %llu is too small for a block of size %llu\n", dst_size, FILESYSTEM_BLOCK_SIZE_IN_BYTES);
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    size_t first_sector_lba_address = DISK_BLOCK_IO_BLOCK_TO_FIRST_SECTOR(block_number);
    for ( size_t sectors_read = 0; sectors_read < FILESYSTEM_BLOCK_SIZE_IN_SECTORS; ++sectors_read )
    {
        size_t offset = sectors_read * DISK_DRIVER_SECTOR_SIZE_IN_BYTES;
        disk_driver_read_sector(first_sector_lba_address + sectors_read, dst + offset, dst_size - offset);
    }
}

void disk_block_io_write_block(size_t block_number, byte* src, size_t src_size)
{

}
