#include <filesystem.h>
#include <disk_block_io.h>

void filesystem_setup(void)
{
    disk_block_io_setup();    
}
