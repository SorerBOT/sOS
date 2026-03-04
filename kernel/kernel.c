#include <io_setup.h>
#include <types.h>
#include <console_io.h>
#include <interrupts.h>
#include <keyboard.h>
#include "keyboard_manager/include/keyboard_manager.h"
#include <keyboard_types.h>

void kernel()
{
    io_setup();
    console_io_report("entered 64-bit long mode...", CONSOLE_IO_SUCCESS);

    interrupts_setup();

    __asm__ volatile ("int $3");
    console_io_report("finished handling a breakpoint interrupt. kernel took back control...", CONSOLE_IO_SUCCESS);

    keyboard_manager_set_layout(KEYBOARD_LAYOUT_US_QWERTY);

    for (;;)
    {
        char c;
        keyboard_read_char(&c);
        console_io_printf("%c", c);
        console_io_flush();
    }

    while (1)
    {
        __asm__ volatile("hlt");
    }
}
