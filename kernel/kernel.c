#include <io_setup.h>
#include <types.h>
#include <console_output.h>
#include <interrupts.h>
#include <tty.h>
#include <keyboard_manager.h>
#include <shell.h>
#include <process_manager.h>
#include <infinite_loop.h>
#include <syscall.h>

static void shell_launch_wrapper(void* _)
{
    shell_launch();
}

static void infinite_loop_launch_wrapper(void* _)
{
    infinite_loop_launch();
}

static void kernel_internal(void* _)
{
    console_output_report("started up the kernel process.", CONSOLE_OUTPUT_SUCCESS);


    syscall_launch_process(shell_launch_wrapper);
    process_manager_launch_process(shell_launch_wrapper);

    while (1)
    {
        __asm__ volatile("hlt");
    }
}

void kernel()
{
    io_setup();

    console_output_report("entered 64-bit long mode.", CONSOLE_OUTPUT_SUCCESS);

    interrupts_setup();

    console_output_printf("To context switch, press control + c\n");

    process_manager_launch_process(kernel_internal);

    while (1)
    {
        __asm__ volatile("hlt");
    }
}

