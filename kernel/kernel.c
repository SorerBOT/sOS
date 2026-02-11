#include <stdbool.h>
#include <vga_driver.h>

void kernel()
{
    VGA_DRIVER_settings_t settings =
    {
        .should_copy_vga_buffer = true,
        .initial_line = 19
    };

    VGA_DRIVER_init(&settings);
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);

    while (1)
    {

        VGA_DRIVER_report("testing VGA scrolling...", VGA_DRIVER_SUCCESS);
        for (volatile size_t i = 0; i < 100000000; ++i)
        {
        }
    }
}
