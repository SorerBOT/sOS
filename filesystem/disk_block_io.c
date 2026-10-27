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

    if ( dst_size == 0 )
    {
        return;
    }

    if ( dst == NULL )
    {
        console_output_print_blue_screen("Disk Block IO Error: provided dst buffer is NULL\n");
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    size_t bytes_to_read = ( dst_size < FILESYSTEM_BLOCK_SIZE_IN_BYTES )
        ? dst_size
        : FILESYSTEM_BLOCK_SIZE_IN_BYTES;

    size_t first_sector_lba_address = DISK_BLOCK_IO_BLOCK_TO_FIRST_SECTOR(block_number);
    for (
            size_t sectors_read = 0, bytes_read = 0;
            bytes_read < bytes_to_read;
            ++sectors_read, bytes_read += DISK_DRIVER_SECTOR_SIZE_IN_BYTES )
    {
        disk_driver_read_sector(first_sector_lba_address + sectors_read, dst + bytes_read, bytes_to_read - bytes_read);
    }
}

void disk_block_io_write_block(size_t block_number, byte* src, size_t src_size)
{
    if ( block_number >= blocks_count )
    {
        console_output_print_blue_screen("Disk Block IO Error: provided block number of %llu is out of range for a disk with a block count of %llu\n", block_number, blocks_count);
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    if ( src_size == 0 )
    {
        return;
    }

    if ( src == NULL )
    {
        console_output_print_blue_screen("Disk Block IO Error: provided src buffer is NULL\n");
        while ( 1 )
        {
            __asm__ volatile("hlt");
        }
    }

    size_t bytes_to_write = ( src_size < FILESYSTEM_BLOCK_SIZE_IN_BYTES )
        ? src_size
        : FILESYSTEM_BLOCK_SIZE_IN_BYTES;

    size_t first_sector_lba_address = DISK_BLOCK_IO_BLOCK_TO_FIRST_SECTOR(block_number);
    for (
            size_t sectors_read = 0, bytes_written = 0;
            bytes_written < bytes_to_write;
            ++sectors_read, bytes_written += DISK_DRIVER_SECTOR_SIZE_IN_BYTES
            )
    {
        disk_driver_write_sector(first_sector_lba_address + sectors_read, src + bytes_written, bytes_to_write - bytes_written);
    }
}
