#ifndef ATA_DRIVER_H
#define ATA_DRIVER_H

#include <types.h>

void ata_driver_setup(void);
void ata_driver_read_sector(size_t lba_address);

#endif /* ATA_DRIVER_H */
