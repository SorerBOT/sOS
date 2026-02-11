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

    size_t print_count = 0;
    while (1)
    {
// TODO: add %lu to printf. This is a little messed up
        VGA_DRIVER_printf("testing VGA scrolling. Print number: %ld...\n", print_count);
        ++print_count;
        if (print_count % 100 == 0)
        {
            VGA_DRIVER_clear();
        }
        for (volatile size_t i = 0; i < 100000000; ++i)
        {
        }
    }
}
