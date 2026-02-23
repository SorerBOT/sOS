#include "include/isr.h"
#include <console_io.h>

enum
{
    ISR_BREAKPOINT = 0x03,
    ISR_PAGE_FAULT = 0x0E
};

static void isr_dump_registers(const isr_args_t* args);

static void isr_dump_registers(const isr_args_t* args)
{
    console_io_print_blue_screen(
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
            "rdi", args->general_registers.rdi, "rsi", args->general_registers.rsi,
            "rax", args->general_registers.rax, "rbx", args->general_registers.rbx,
            "rcx", args->general_registers.rcx, "rdx", args->general_registers.rdx,
            "r8", args->general_registers.r8, "r9", args->general_registers.r9,
            "r10", args->general_registers.r10, "r11", args->general_registers.r11,
            "r12", args->general_registers.r12, "r13", args->general_registers.r13,
            "r14", args->general_registers.r12, "r15", args->general_registers.r13);
}

void isr_handler(isr_args_t* args)
{
    switch ( args->isr_number )
    {
        case ISR_BREAKPOINT:
            console_io_print_blue_screen("Breakpoint on instruction %p reached.\n", args->rip);
            isr_dump_registers(args);
            break;
        case ISR_PAGE_FAULT:
            break;
    }
}
