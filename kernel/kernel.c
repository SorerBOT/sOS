#define VGA_DRIVER_INITIAL_LINE 19
#define VGA_DRIVER_IMPLEMENTATION
#include "../drivers/vga_driver.h"

void kernel()
{
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);

    while (1);
}
