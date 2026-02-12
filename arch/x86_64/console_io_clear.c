#include <vga_driver.h>
#include <console_io.h>

void console_io_clear()
{
    vga_driver_clear_colored(VGA_DRIVER_COLOR_DEFAULT);
}
