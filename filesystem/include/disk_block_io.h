#ifndef DISK_BLOCK_IO
#define DISK_BLOCK_IO

#include <types.h>

void disk_block_io_setup(void);

void disk_block_io_read_block(size_t block_number, byte* dst, size_t dst_size);
void disk_block_io_write_block(size_t block_number, byte* src, size_t src_size);

void disk_block_io_read_blocks_from(size_t base_block, byte* dst, size_t dst_size);
void disk_block_io_write_blocks_from(size_t base_block, byte* src, size_t src_size);

#endif /* DISK_BLOCK_IO */
