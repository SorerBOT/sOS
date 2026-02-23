#include <console_io.h>
#include <vga_driver.h>
#include <stdarg.h>
#include <stdio.h>

static char console_io_buffer[VGA_DRIVER_SHADOW_MAX_CHARS];
static bool is_blue_screen = false; // this is a terminal state. persists until reboot

static void console_io__internal_vprintf_colored(byte color, const char* restrict format, va_list ap)
{
    va_list ap_copy;
    va_copy(ap_copy, ap);
    vsnprintf(console_io_buffer, VGA_DRIVER_SHADOW_MAX_CHARS, format, ap_copy);

    vga_driver_print_string_colored(color, console_io_buffer);
}
static void console_io__internal_printf_colored(byte color, const char* restrict format, ...)
{
    va_list ap;
    va_start(ap, format);
    console_io__internal_vprintf_colored(color, format, ap);
    va_end(ap);
}

void console_io_printf(const char * restrict format, ...)
{
    va_list ap;
    va_start(ap, format);
    byte color = is_blue_screen
        ? VGA_DRIVER_COLOR_BLUE_SCREEN
        : VGA_DRIVER_COLOR_DEFAULT;
    console_io__internal_vprintf_colored(color, format, ap);
    va_end(ap);
}

void console_io_print_blue_screen(const char* restrict format, ...)
{
    is_blue_screen = true;
    vga_driver_clear_colored(VGA_DRIVER_COLOR_BLUE_SCREEN);
    vga_driver_print_string_colored(VGA_DRIVER_COLOR_BLUE_SCREEN, "An error has occurred:\n");

    va_list ap;
    va_start(ap, format);
    console_io__internal_vprintf_colored(VGA_DRIVER_COLOR_BLUE_SCREEN, format, ap);
    va_end(ap);

    vga_driver_flush_shadow_buffer(); // just in case the user didn't add \n
}

void console_io_flush()
{
    vga_driver_flush_shadow_buffer();
}

void console_io_clear()
{
    vga_driver_clear_colored(VGA_DRIVER_COLOR_DEFAULT);
}

void console_io_report(const char* restrict message, console_io_report_status_t status)
{
    console_io_printf("[");
    if (status == CONSOLE_IO_SUCCESS)
    {
        console_io__internal_printf_colored(VGA_DRIVER_COLOR_SUCCESS, " SUCCESS ");
    }
    else if (status == CONSOLE_IO_FAILURE)
    {
        console_io__internal_printf_colored(VGA_DRIVER_COLOR_FAILURE, " FAILURE ");
    }

    console_io_printf("] %s\n", message);
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
