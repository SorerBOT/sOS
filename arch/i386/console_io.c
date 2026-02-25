#include <console_io.h>
#include <vga_driver.h>
#include <stdarg.h>

void console_io_report(const char* restrict message, console_io_report_status_t status)
{
    vga_driver_print_string_colored(VGA_DRIVER_COLOR_DEFAULT, "[");
    if (status == CONSOLE_IO_SUCCESS)
    {
        vga_driver_print_string_colored(VGA_DRIVER_COLOR_SUCCESS, " SUCCESS ");
    }
    else if (status == CONSOLE_IO_FAILURE)
    {
        vga_driver_print_string_colored(VGA_DRIVER_COLOR_FAILURE, " FAILURE ");
    }

    vga_driver_print_string_colored(VGA_DRIVER_COLOR_DEFAULT, "] ");
    vga_driver_print_string_colored(VGA_DRIVER_COLOR_DEFAULT, message);
    vga_driver_print_string_colored(VGA_DRIVER_COLOR_DEFAULT, "\n");
}

void console_io_init(const console_io_init_settings_t* settings)
{
    vga_driver_settings_t vga_settings =
    {
        .initial_line = settings->initial_line,
        .should_copy_vga_buffer = settings->should_copy_existing_buffer
    };

    vga_driver_init(&vga_settings);
}

void console_io_printf(const char* restrict format, ...)
{
    console_io_report("console_io_printf: not supported in this architecture.\n", CONSOLE_IO_FAILURE);
}

void console_io_clear()
{
    console_io_report("console_io_clear: not supported in this architecture.\n", CONSOLE_IO_FAILURE);
}

void console_io_print_blue_screen(const char* restrict format, ...)
{
    console_io_report("console_io_print_blue_screen: not supported in this architecture.\n", CONSOLE_IO_FAILURE);
}

void console_io_flush()
{
    console_io_report("console_io_flush: not supported in this architecture.\n", CONSOLE_IO_FAILURE);
}
