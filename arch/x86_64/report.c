#include <stdio.h>
#include <vga_driver.h>

void report(const char* restrict message, stdio_report_status_t status)
{
    VGA_DRIVER_PRINTF("[");
    if (status == STDIO_SUCCESS)
    {
        vga_driver_printf_colored(VGA_DRIVER_COLOR_SUCCESS, " SUCCESS ");
    }
    else if (status == STDIO_FAILURE)
    {
        vga_driver_printf_colored(VGA_DRIVER_COLOR_FAILURE, " FAILURE ");
    }

    VGA_DRIVER_PRINTF("] %s\n", message);
}
