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
#include <gdt.h>
#include <kernel_allocator.h>

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

    gdt_setup();
    console_output_report("finished settting up the kernel GDT.", CONSOLE_OUTPUT_SUCCESS);

    interrupts_setup();
    console_output_report("finished settting up interrupts.", CONSOLE_OUTPUT_SUCCESS);

    pmm_setup();
    console_output_report("finished settting up the physical memory allocator.", CONSOLE_OUTPUT_SUCCESS);

    vmm_setup();
    console_output_report("finished settting up the virtual memory allocator.", CONSOLE_OUTPUT_SUCCESS);

    kernel_allocator_setup();
    console_output_report("finished settting up the kernel allocator.", CONSOLE_OUTPUT_SUCCESS);

    syscall_dispatcher_launch_process(kernel_internal);

    while (1)
    {
        __asm__ volatile("hlt");
    }
}

