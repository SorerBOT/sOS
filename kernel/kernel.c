#include <stdbool.h>
#include <vga_driver.h>

void kernel()
{
    vga_driver_settings_t settings =
    {
        .should_copy_vga_buffer = true,
        .initial_line = 19
    };

    vga_driver_init(&settings);
    vga_driver_report("entered 64-bit long mode...", VGA_DRIVER_SUCCESS);

    size_t print_count = 0;
    while (1)
    {
// TODO: add %lu to printf. This is a little messed up
        vga_driver_printf("testing VGA scrolling. Print number: %ld...\n", print_count);
        ++print_count;
        if (print_count % 15 == 0)
        {
            vga_driver_clear();
        }
        for (volatile size_t i = 0; i < 100000000; ++i)
        {
        }
        if (print_count == 50)
        {
            vga_driver_print_blue_screen("You've been found guilty of printing useless stuff! %ld prints, are you crazy?\n", print_count);
            while (1);
        }
    }
}
