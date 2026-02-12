#include <stdio.h>
#include <vga_driver.h>

void stdio_init(const stdio_init_settings_t* settings)
{
    vga_driver_settings_t vga_settings =
    {
        .initial_line = settings->initial_line,
        .should_copy_vga_buffer = settings->should_copy_existing_buffer
    };

    vga_driver_init(&vga_settings);
}
