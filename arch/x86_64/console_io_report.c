#include <console_io.h>
#include <vga_driver.h>

void console_io_report(const char* restrict message, console_io_report_status_t status)
{
    VGA_DRIVER_PRINTF("[");
    if (status == CONSOLE_IO_SUCCESS)
    {
        vga_driver_printf_colored(VGA_DRIVER_COLOR_SUCCESS, " SUCCESS ");
    }
    else if (status == CONSOLE_IO_FAILURE)
    {
        vga_driver_printf_colored(VGA_DRIVER_COLOR_FAILURE, " FAILURE ");
    }

    VGA_DRIVER_PRINTF("] %s\n", message);
}
