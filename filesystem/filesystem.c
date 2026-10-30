#include <string.h>
#include <filesystem.h>
#include <disk_block_io.h>
#include <console_output.h>
#include <ext2.h>


void filesystem_setup(void)
{
    disk_block_io_setup();
    ext2_setup();
}
