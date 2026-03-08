#include <io_setup.h>
#include <types.h>
#include <console_output.h>
#include <interrupts.h>
#include <tty.h>
#include <keyboard_manager.h>
#include <shell.h>

void kernel()
{
    io_setup();

    console_output_report("entered 64-bit long mode.", CONSOLE_OUTPUT_SUCCESS);

    interrupts_setup();

    shell_launch();

    while (1)
    {
        __asm__ volatile("hlt");
    }
}
