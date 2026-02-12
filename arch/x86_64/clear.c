#include <vga_driver.h>
#include <stdio.h>

void clear()
{
    vga_driver_clear_colored(VGA_DRIVER_COLOR_DEFAULT);
}
