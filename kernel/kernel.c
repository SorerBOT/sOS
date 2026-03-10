#include <io_setup.h>
#include <types.h>
#include <console_output.h>
#include <interrupts.h>
#include <tty.h>
#include <keyboard_manager.h>
#include <shell.h>
#include <process_manager.h>

static void shell_launch_wrapper(void* _)
{
    shell_launch();
}

void kernel()
{
    io_setup();

    console_output_report("entered 64-bit long mode.", CONSOLE_OUTPUT_SUCCESS);

    interrupts_setup();

    process_manager_launch_process(shell_launch_wrapper);

    while (1)
    {
        __asm__ volatile("hlt");
    }
}
