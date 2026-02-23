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

    __asm__ volatile ("int $3");
    console_io_report("finished handling a breakpoint interrupt. kernel took back control...", CONSOLE_IO_SUCCESS);


    while (1)
    {
        __asm__ volatile("cli; hlt");
    }
}
