#ifndef DISK_DRIVER_H
#define DISK_DRIVER_H

#include <types.h>

#define DISK_DRIVER_SECTOR_SIZE_IN_WORDS 256
#define DISK_DRIVER_SECTOR_SIZE_IN_BYTES ((DISK_DRIVER_SECTOR_SIZE_IN_WORDS) * 2)

#define disk_driver_setup ata_driver_setup
#define disk_driver_read_sector ata_driver_read_sector
#define disk_driver_write_sector ata_driver_write_sector


#endif /* DISK_DRIVER_H */
