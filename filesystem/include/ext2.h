#ifndef EXT2_H
#define EXT2_H

#define EXT2_BLOCK_SIZE_IN_SECTORS 8
#define EXT2_FIRST_BLOCK_OFFSET 1024
#define EXT2_BLOCK_SIZE_IN_WORDS ((EXT2_BLOCK_SIZE_IN_SECTORS) * (DISK_DRIVER_SECTOR_SIZE_IN_WORDS))
#define EXT2_BLOCK_SIZE_IN_BYTES ((EXT2_BLOCK_SIZE_IN_WORDS) * 2)

void ext2_setup(void);
void ext2_print_directory_contents(void);

#endif /* EXT2_H */
