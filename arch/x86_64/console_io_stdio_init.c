#include <console_io.h>
#include <vga_driver.h>

void console_io_init(const console_io_init_settings_t* settings)
{
    vga_driver_settings_t vga_settings =
    {
        .initial_line = settings->initial_line,
        .should_copy_vga_buffer = settings->should_copy_existing_buffer
    };

    vga_driver_init(&vga_settings);
}
