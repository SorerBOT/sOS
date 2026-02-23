#include <stdbool.h>
#include <console_io.h>
#include <interrupts.h>

void kernel()
{
    console_io_init_settings_t settings =
    {
        .initial_line = 19,
            .should_copy_existing_buffer = true
    };

    console_io_init(&settings);
    console_io_report("entered 64-bit long mode...", CONSOLE_IO_SUCCESS);

    interrupts_setup();

    console_io_report("set up interrupts...", CONSOLE_IO_SUCCESS);


    __asm__ volatile ("int $3");


    while (1);
}
