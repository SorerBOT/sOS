#include <io_setup.h>
#include <types.h>
#include <console_output.h>
#include <interrupts.h>
#include <tty.h>
#include <keyboard_manager.h>
#include <shell.h>
#include <process_manager.h>
#include <syscall_dispatcher.h>
#include <pmm.h>
#include <vmm.h>

static void shell_launch_wrapper(void* _)
{
    shell_launch();
}

static void kernel_internal(void* _)
{
    console_output_report("started up the kernel process.", CONSOLE_OUTPUT_SUCCESS);

    syscall_dispatcher_launch_process(shell_launch_wrapper);

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

    pmm_setup();
    vmm_setup();

    dword* buggy_address = (dword*)(0x7FCFFF00);
    *buggy_address = 0xdeadbeef;


    syscall_dispatcher_launch_process(kernel_internal);

    while (1)
    {
        __asm__ volatile("hlt");
    }
}

