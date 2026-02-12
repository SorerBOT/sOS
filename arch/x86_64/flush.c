#include <stdio.h>
#include <vga_driver.h>

void flush()
{
    vga_driver_flush_shadow_buffer();
}
