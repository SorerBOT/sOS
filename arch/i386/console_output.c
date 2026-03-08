#include <console_output.h>
#include <vga_driver.h>

void console_output_report(const char* restrict message, console_output_report_status_t status)
{
    vga_driver_print_string_colored(VGA_DRIVER_COLOR_DEFAULT, "[");
    if (status == CONSOLE_OUTPUT_SUCCESS)
    {
        vga_driver_print_string_colored(VGA_DRIVER_COLOR_SUCCESS, " SUCCESS ");
    }
    else if (status == CONSOLE_OUTPUT_FAILURE)
    {
        vga_driver_print_string_colored(VGA_DRIVER_COLOR_FAILURE, " FAILURE ");
    }

    vga_driver_print_string_colored(VGA_DRIVER_COLOR_DEFAULT, "] ");
    vga_driver_print_string_colored(VGA_DRIVER_COLOR_DEFAULT, message);
    vga_driver_print_string_colored(VGA_DRIVER_COLOR_DEFAULT, "\n");
}

void console_output_init(const console_output_init_settings_t* settings)
{
    vga_driver_settings_t vga_settings =
    {
        .initial_line = settings->initial_line,
        .should_copy_vga_buffer = settings->should_copy_existing_buffer
    };

    vga_driver_init(&vga_settings);
}

void console_output_printf(const char* restrict format, ...)
{
    console_output_report("console_output_printf: not supported in this architecture.\n", CONSOLE_OUTPUT_FAILURE);
}

void console_output_clear()
{
    console_output_report("console_output_clear: not supported in this architecture.\n", CONSOLE_OUTPUT_FAILURE);
}

void console_output_print_blue_screen(const char* restrict format, ...)
{
    console_output_report("console_output_print_blue_screen: not supported in this architecture.\n", CONSOLE_OUTPUT_FAILURE);
}

void console_output_flush()
{
    console_output_report("console_output_flush: not supported in this architecture.\n", CONSOLE_OUTPUT_FAILURE);
}

void console_output_backspace(void)
{
    console_output_report("console_output_backspace: not supported in this architecture.\n", CONSOLE_OUTPUT_FAILURE);
}
