#include <stdbool.h>
#include <stdio.h>

void kernel()
{
    stdio_init_settings_t settings =
    {
        .initial_line = 19,
            .should_copy_existing_buffer = true
    };

    stdio_init(&settings);
    report("entered 64-bit long mode...", STDIO_SUCCESS);

    size_t print_count = 0;
    while (1)
    {
// TODO: add %lu to printf. This is a little messed up
        printf("testing VGA scrolling. Print number: %ld...\n", print_count);
        ++print_count;
        if (print_count % 15 == 0)
        {
            clear();
        }
        for (volatile size_t i = 0; i < 100000000; ++i)
        {
        }
        if (print_count == 50)
        {
            print_blue_screen("You've been found guilty of printing useless stuff! %ld prints, are you crazy?\n", print_count);
            while (1);
        }
    }
}
