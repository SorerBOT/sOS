#include <stdio.h>
#include <vga_driver.h>

void print_blue_screen(const char* restrict format, ...)
{
    vga_driver_clear_colored(VGA_DRIVER_COLOR_BLUE_SCREEN);
    vga_driver_printf_colored(VGA_DRIVER_COLOR_BLUE_SCREEN, "An error has occurred:\n");

    va_list ap;
    va_start(ap, format);

    vga_driver_vprintf_colored(VGA_DRIVER_COLOR_BLUE_SCREEN, format, ap);

    va_end(ap);

    vga_driver_flush_shadow_buffer(); // just in case the user didn't add \n
}
