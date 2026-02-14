#include <stdbool.h>
#include <console_io.h>

void kernel()
{
    console_io_init_settings_t settings =
    {
        .initial_line = 19,
            .should_copy_existing_buffer = true
    };

    console_io_init(&settings);
    console_io_report("entered 64-bit long mode...", CONSOLE_IO_SUCCESS);

    //console_io_printf("%llx\n", 3735928559);

    //while(1);

    size_t print_count = 0;

    while (1)
    {
// TODO: add %lu to printf. This is a little messed up
        console_io_printf("testing VGA scrolling. Print number: %ld...\n", print_count);
        ++print_count;
        if (print_count % 15 == 0)
        {
            console_io_clear();
        }
        for (volatile size_t i = 0; i < 100000000; ++i)
        {
        }
        if (print_count == 50)
        {
            console_io_print_blue_screen("You've been found guilty of printing useless stuff! %ld prints, are you crazy?\n", print_count);
            while (1);
        }
    }
}
