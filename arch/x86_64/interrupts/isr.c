#include "include/isr.h"
#include "include/idt.h"
#include "include/pic.h"
#include <console_io.h>
#include <cpu_io.h>

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
static void isr_handler_general_protection_fault(const isr_args_t* args);
static void isr_handler_pic_interrupts(const isr_args_t* args);
static bool isr_is_pic_interrupt(qword isr_number);

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

static void isr_handler_general_protection_fault(const isr_args_t* args)
{
    console_io_print_blue_screen("General Protection Fault Occurred\n");
    if ( args->error_code == 0 )
    {
        console_io_printf("The fault is not segment related\n");
    }
    else
    {
        console_io_printf("Segment violation error at segment %llu\n", args->error_code);
    }
}

static bool isr_is_pic_interrupt(qword isr_number)
{
    return ((isr_number >= IDT_OFFSET_PIC_MASTER && isr_number < IDT_OFFSET_PIC_MASTER + 8)
        || (isr_number >= IDT_OFFSET_PIC_SLAVE && isr_number < IDT_OFFSET_PIC_SLAVE + 8));
}

static void isr_handler_pic_interrupts(const isr_args_t* args)
{
    uint8_t irq_number;
    uint8_t isr_number = (uint8_t) args->isr_number;

    /* even though I placed the slave PIC right after the master PIC, this should not depend on it.
     * I might want to support multiple slave PICs in the future, and I wanted to make it obvious
     * where changes would be needed.
     */
    if ( isr_number >= IDT_OFFSET_PIC_MASTER
            && isr_number < IDT_OFFSET_PIC_MASTER + PIC_IRQ_COUNT_PER_UNIT )
    {
        irq_number = isr_number - IDT_OFFSET_PIC_MASTER;
    }
    else
    {
        irq_number = PIC_IRQ_COUNT_PER_UNIT + isr_number - IDT_OFFSET_PIC_SLAVE;
    }


    if ( irq_number == 1 )
    {
        console_io_printf("KEYBOARD PRESSED\n");
        cpu_io_inb(0x60);
    }


    pic_send_EOI(irq_number);
}


void isr_handler(isr_args_t* args)
{
    if ( isr_is_pic_interrupt(args->isr_number) )
    {
        isr_handler_pic_interrupts(args);
        return;
    }

    switch ( args->isr_number )
    {
        case ISR_DIVIDE_BY_ZERO:
            console_io_print_blue_screen("Divide by zero occurred:\n");
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
            console_io_print_blue_screen("Double fault occurred:\n");
            isr_dump_registers(args);
            while (1)
            {
                __asm__ volatile("cli; hlt");
            }
            break;
        case ISR_PAGE_FAULT:
            isr_handler_page_fault(args);
            break;
        case ISR_GENERAL_PROTECTION_FAULT:
            isr_handler_general_protection_fault(args);
            break;
    }
}
