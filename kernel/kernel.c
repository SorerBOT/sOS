#include <io_setup.h>
#include <stdbool.h>
#include <console_io.h>
#include <interrupts.h>
#include <ps2_keyboard.h>

void kernel()
{
    io_setup();
    console_io_report("entered 64-bit long mode...", CONSOLE_IO_SUCCESS);

    interrupts_setup();

    __asm__ volatile ("int $3");
    console_io_report("finished handling a breakpoint interrupt. kernel took back control...", CONSOLE_IO_SUCCESS);

    for (;;)
    {
        char c;
        ps2_keyboard_read_char_from_ring_buffer(&c);
        console_io_printf("%c", c);
        console_io_flush();
    }

    while (1)
    {
        __asm__ volatile("hlt");
    }
}
