#ifndef ATA_DRIVER_H
#define ATA_DRIVER_H

#include <types.h>
#include <disk_types.h>

void ata_driver_setup(void);
void ata_driver_read_sector(size_t lba_address, disk_sector_t* dst);

#endif /* ATA_DRIVER_H */
