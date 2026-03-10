#include "include/isr.h"
#include "include/idt.h"
#include "include/pic.h"

#include <console_output.h>
#include <cpu_io.h>
#include <ps2_keyboard_driver.h>
#include <keyboard_types.h>
#include <keyboard_driver.h>
#include <process_types.h>
#include <process_manager.h>

enum
{
    ISR_DIVIDE_BY_ZERO              = 0x00,
    ISR_BREAKPOINT                  = 0x03,
    ISR_INVALID_OPCODE              = 0x06,
    ISR_DOUBLE_FAULT                = 0x08,
    ISR_PAGE_FAULT                  = 0x0E,
    ISR_GENERAL_PROTECTION_FAULT    = 0x0D,
    ISR_SYSCALL                     = 0x80
};

static void handler_context_switch(isr_args_t* args);
static void dump_registers(const isr_args_t* args);
static void handler_page_fault(const isr_args_t* args);
static void handler_general_protection_fault(const isr_args_t* args);
static void handler_pic_interrupts(isr_args_t* args);
static bool is_pic_interrupt(qword isr_number);
static void handler_syscall(isr_args_t* args);

static void handler_context_switch(isr_args_t* args)
{
    process_context_t current_context =
    {
        .registers =
        {
            .r15 = args->general_registers.r15,
            .r14 = args->general_registers.r14,
            .r13 = args->general_registers.r13,
            .r12 = args->general_registers.r12,
            .r11 = args->general_registers.r11,
            .r10 = args->general_registers.r10,
            .r9 = args->general_registers.r9,
            .r8 = args->general_registers.r8,
            .rbp = args->general_registers.rbp,
            .rsp = args->rsp,
            .rdi = args->general_registers.rdi,
            .rsi = args->general_registers.rsi,
            .rip = args->rip,
            .rax = args->general_registers.rax,
            .rbx = args->general_registers.rbx,
            .rcx = args->general_registers.rcx,
            .rdx = args->general_registers.rdx
        }
    };

    process_id_t current_pid = process_manager_get_running_process_idx();
    size_t processes_count = process_manager_get_processes_count();
    process_id_t next_process = (current_pid + 1) % processes_count;

    const process_context_t* new_context = process_manager_context_switch(next_process, current_context);

    console_output_printf("PID: %lu, RIP: %p, RSP: %p, RBP: %p\n",
            new_context->pid,
            new_context->registers.rip,
            new_context->registers.rsp, new_context->registers.rbp);

    args->general_registers.r15 = new_context->registers.r15;
    args->general_registers.r14 = new_context->registers.r14;
    args->general_registers.r13 = new_context->registers.r13;
    args->general_registers.r12 = new_context->registers.r12;
    args->general_registers.r11 = new_context->registers.r11;
    args->general_registers.r10 = new_context->registers.r10;
    args->general_registers.r9 = new_context->registers.r9;
    args->general_registers.r8 = new_context->registers.r8;
    args->general_registers.rsi = new_context->registers.rsi;
    args->general_registers.rdx = new_context->registers.rdx;
    args->general_registers.rcx = new_context->registers.rcx;
    args->general_registers.rbx = new_context->registers.rbx;
    args->general_registers.rax = new_context->registers.rax;
    args->general_registers.rdi = new_context->registers.rdi;
    args->general_registers.rbp = new_context->registers.rbp;
    args->rip = new_context->registers.rip;
    args->rsp = new_context->registers.rsp;

    console_output_printf("PID: %lu, RIP: %p, RSP: %p, RBP: %p\n",
            new_context->pid,
            args->rip,
            args->rsp,
            args->general_registers.rbp);
}

static void dump_registers(const isr_args_t* args)
{
    process_id_t pid = process_manager_get_running_process_idx();
    console_output_printf(
            "Registers dump for process: %lu:\n"
            "%3s: %0x\n"
            "%3s: %016llx        %3s: %016llx\n"
            "%3s: %016llx        %3s: %016llx\n"
            "%3s: %016llx        %3s: %016llx\n"
            "%3s: %016llx        %3s: %016llx\n"
            "%3s: %016llx        %3s: %016llx\n"
            "%3s: %016llx        %3s: %016llx\n"
            "%3s: %016llx        %3s: %016llx\n"
            "%3s: %016llx        %3s: %016llx\n",
            pid,
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

static void handler_page_fault(const isr_args_t* args)
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

    console_output_print_blue_screen(
            "Page Fault Occurred\n"
            "Faulting address: %p\n", faulting_address);

    dump_registers(args);

    console_output_printf(
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

static void handler_general_protection_fault(const isr_args_t* args)
{
    console_output_print_blue_screen("General Protection Fault Occurred\n");
    if ( args->error_code == 0 )
    {
        console_output_printf("The fault is not segment related\n");
    }
    else
    {
        console_output_printf("Segment violation error at segment %llu\n", args->error_code);
    }
}

static bool is_pic_interrupt(qword isr_number)
{
    return ((isr_number >= IDT_OFFSET_PIC_MASTER && isr_number < IDT_OFFSET_PIC_MASTER + 8)
        || (isr_number >= IDT_OFFSET_PIC_SLAVE && isr_number < IDT_OFFSET_PIC_SLAVE + 8));
}

static void handler_pic_interrupts(isr_args_t* args)
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
        ps2_keyboard_driver_read_and_handle_scancode();

        bool is_control_pressed = keyboard_driver_get_key_state(KEYBOARD_KEYCODE_CONTROL_L);
        bool is_c_pressed = keyboard_driver_get_key_state(KEYBOARD_KEYCODE_C);
        if ( is_control_pressed && is_c_pressed )
        {
            handler_context_switch(args);
        }
    }


    pic_send_EOI(irq_number);
}

static void handler_syscall(isr_args_t* args)
{
}

void isr_handler(isr_args_t* args)
{
    if ( is_pic_interrupt(args->isr_number) )
    {
        handler_pic_interrupts(args);
        return;
    }

    switch ( args->isr_number )
    {
        case ISR_DIVIDE_BY_ZERO:
            console_output_print_blue_screen("Divide by zero occurred:\n");
            dump_registers(args);
            while (1)
            {
                __asm__ volatile("cli; hlt");
            }
            break;
        case ISR_BREAKPOINT:
            console_output_printf("Breakpoint on instruction %p reached.\n", args->rip);
            break;
        case ISR_DOUBLE_FAULT:
            console_output_print_blue_screen("Double fault occurred:\n");
            dump_registers(args);
            while (1)
            {
                __asm__ volatile("cli; hlt");
            }
            break;
        case ISR_PAGE_FAULT:
            handler_page_fault(args);
            while (1)
            {
                __asm__ volatile("cli; hlt");
            }
            break;
        case ISR_GENERAL_PROTECTION_FAULT:
            handler_general_protection_fault(args);
            while (1)
            {
                __asm__ volatile("cli; hlt");
            }
            break;
        case ISR_SYSCALL:
            handler_syscall(args);
            break;
        case ISR_INVALID_OPCODE:
            console_output_print_blue_screen("Invalid opcode:\n");
            dump_registers(args);
            while (1)
            {
                __asm__ volatile("cli; hlt");
            }
            break;
    }
}
