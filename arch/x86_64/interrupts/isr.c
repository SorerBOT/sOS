#include "include/isr.h"
#include <console_io.h>

enum
{
    ISR_DIVIDE_BY_ZERO = 0x00,
    ISR_BREAKPOINT = 0x03,
    ISR_DOUBLE_FAULT = 0x08,
    ISR_PAGE_FAULT = 0x0E,
    ISR_GENERAL_PROTECTION_FAULT = 0x0D
};

static void isr_dump_registers(const isr_args_t* args);
static void isr_handler_page_fault(const isr_args_t* args);

static void isr_dump_registers(const isr_args_t* args)
{
    console_io_printf(
            "Registers dump:\n"
            "%3s: %08x\n"
            "%3s: %08x        %3s: %08x\n"
            "%3s: %08x        %3s: %08x\n"
            "%3s: %08x        %3s: %08x\n"
            "%3s: %08x        %3s: %08x\n"
            "%3s: %08x        %3s: %08x\n"
            "%3s: %08x        %3s: %08x\n"
            "%3s: %08x        %3s: %08x\n"
            "%3s: %08x        %3s: %08x\n",
            "rip", args->rip,
            "cs", args->cs, "ss", args->ss,
            "rsp", args->rsp, "rbp", args->general_registers.rbp,
            "rdi", args->general_registers.rdi, "rsi", args->general_registers.rsi,
            "rax", args->general_registers.rax, "rbx", args->general_registers.rbx,
            "rcx", args->general_registers.rcx, "rdx", args->general_registers.rdx,
            "r8", args->general_registers.r8, "r9", args->general_registers.r9,
            "r10", args->general_registers.r10, "r11", args->general_registers.r11,
            "r12", args->general_registers.r12, "r13", args->general_registers.r13,
            "r14", args->general_registers.r12, "r15", args->general_registers.r13);
}

static void isr_handler_page_fault(const isr_args_t* args)
{
    void* faulting_address;
    __asm__ volatile("mov %%cr2, %0" : "=r" (faulting_address));

    bool is_present                     = (args->error_code & 0b1) != 0;
    bool is_write                       = (args->error_code & 0b10) != 0;
    bool is_user                        = (args->error_code & 0b100) != 0;
    bool is_reserved                    = (args->error_code & 0b1000) != 0;
    bool is_instruction_fetch           = (args->error_code & 0b10000) != 0;
    bool is_protection_key              = (args->error_code & 0b100000) != 0;
    bool is_shadow_stack                = (args->error_code & 0b1000000) != 0;
    bool is_software_guard_extension    = (args->error_code & 0b10000000) != 0;

    console_io_print_blue_screen(
            "Page Fault Occurred\n"
            "Faulting address: %p\n", faulting_address);

    isr_dump_registers(args);

    console_io_printf(
            "%27s:    %d\n"
            "%27s:    %d\n"
            "%27s:    %d\n"
            "%27s:    %d\n"
            "%27s:    %d\n"
            "%27s:    %d\n"
            "%27s:    %d\n"
            "%27s:    %d\n",
            "is_present", is_present,
            "is_write", is_write,
            "is_user", is_user,
            "is_reserved", is_reserved,
            "is_instruction_fetch", is_instruction_fetch,
            "is_protection_key", is_protection_key,
            "is_shadow_stack", is_shadow_stack,
            "is_software_guard_extension", is_software_guard_extension);

    while (1)
    {
        __asm__ volatile("cli; hlt");
    }
}

void isr_handler(isr_args_t* args)
{
    switch ( args->isr_number )
    {
        case ISR_DIVIDE_BY_ZERO:
            console_io_print_blue_screen("DIVIDE BY ZERO OCCURRED:\n");
            isr_dump_registers(args);
            while (1)
            {
                __asm__ volatile("cli; hlt");
            }
            break;
        case ISR_BREAKPOINT:
            console_io_printf("Breakpoint on instruction %p reached.\n", args->rip);
            break;
        case ISR_DOUBLE_FAULT:
            console_io_print_blue_screen("DOUBLE FAULT OCCURRED:\n");
            isr_dump_registers(args);
            while (1)
            {
                __asm__ volatile("cli; hlt");
            }
            break;
        case ISR_PAGE_FAULT:
            isr_handler_page_fault(args);
            break;
    }
}
