#ifndef ATA_DRIVER_H
#define ATA_DRIVER_H

#include <types.h>

#define ATA_DRIVER_SECTOR_SIZE_IN_WORDS 256
#define ATA_DRIVER_SECTOR_SIZE_IN_BYTES ((ATA_DRIVER_SECTOR_SIZE_IN_WORDS) * 2)

void ata_driver_setup(void);
void ata_driver_read_sector(size_t lba_address, byte* dst, size_t dst_size);

#endif /* ATA_DRIVER_H */
