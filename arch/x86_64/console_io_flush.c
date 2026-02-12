#include <console_io.h>
#include <vga_driver.h>

void console_io_flush()
{
    vga_driver_flush_shadow_buffer();
}
