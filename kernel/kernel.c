#include <io_setup.h>
#include <types.h>
#include <console_output.h>
#include <interrupts.h>
#include <tty.h>
#include <keyboard_manager.h>

void kernel()
{
    io_setup();

    console_output_report("entered 64-bit long mode.", CONSOLE_OUTPUT_SUCCESS);

    interrupts_setup();

    for (;;)
    {
        char c;
        tty_read_char(&c);
        console_output_printf("%c", c);
        console_output_flush();
    }

    while (1)
    {
        __asm__ volatile("hlt");
    }
}
