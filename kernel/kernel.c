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
    for (volatile size_t i = 0; i < 1000000000; ++i)
    {
    }
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);

    for (volatile size_t i = 0; i < 1000000000; ++i)
    {
    }
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);
    for (volatile size_t i = 0; i < 1000000000; ++i)
    {
    }
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);
    for (volatile size_t i = 0; i < 1000000000; ++i)
    {
    }
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);
    for (volatile size_t i = 0; i < 1000000000; ++i)
    {
    }
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);
    for (volatile size_t i = 0; i < 1000000000; ++i)
    {
    }
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);
    for (volatile size_t i = 0; i < 1000000000; ++i)
    {
    }
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);
    for (volatile size_t i = 0; i < 1000000000; ++i)
    {
    }
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);
    for (volatile size_t i = 0; i < 1000000000; ++i)
    {
    }
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);
    for (volatile size_t i = 0; i < 1000000000; ++i)
    {
    }
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);
    for (volatile size_t i = 0; i < 1000000000; ++i)
    {
    }
    VGA_DRIVER_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);
    for (volatile size_t i = 0; i < 1000000000; ++i)
    {
    }

    while (1);
}
