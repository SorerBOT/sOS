#ifndef ATA_DRIVER_H
#define ATA_DRIVER_H

#include <types.h>

void ata_driver_setup(void);
size_t ata_driver_get_sector_count(void);
void ata_driver_read_sector(size_t lba_address, byte* dst, size_t dst_size);
void ata_driver_write_sector(size_t lba_address, byte* src, size_t src_size);

#endif /* ATA_DRIVER_H */
