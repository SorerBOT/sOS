#include <interrupts.h>
#include "include/isr_wrappers.h"

#include <stddef.h>
#include <types.h>

#define INTERRUPTS_IDT_SIZE 256

#define INTERRUPTS_DESCRIPTOR_SEGMENT_SELECTOR_RPL 0
#define INTERRUPTS_DESCRIPTOR_SEGMENT_SELECTOR_TI 0
#define INTERRUPTS_DESCRIPTOR_SEGMENT_SELECTOR_CODE_SEGMENT_IDX 2
#define INTERRUPTS_DESCRIPTOR_SEGMENT_SELECTOR ((INTERRUPTS_DESCRIPTOR_SEGMENT_SELECTOR_CODE_SEGMENT_IDX << 3) | (INTERRUPTS_DESCRIPTOR_SEGMENT_SELECTOR_TI << 2) | INTERRUPTS_DESCRIPTOR_SEGMENT_SELECTOR_RPL)
#define INTERRUPTS_DESCRIPTOR_IST 0b00
#define INTERRUPTS_DESCRIPTOR_RESERVED 0b000000
#define INTERRUPTS_DESCRIPTOR_GATE_TYPE_INTERRUPT 0xE
#define INTERRUPTS_DESCRIPTOR_GATE_TYPE_TRAP 0xF
#define INTERRUPTS_DESCRIPTOR_DPL 0b00
#define INTERRUPTS_DESCRIPTOR_PRESENT 0b1

#define INTERRUPTS_GET_ISR_ENTRY(interrupt_number) (isr_wrapper_##interrupt_number)


__attribute__((packed))
typedef struct
{
    word offset_low;
    word segment_selector;
    byte ist_plus_reserved_low;
    byte gate_type_plus_zero_plus_dpl_plus_p;
    word offset_middle;
    dword offset_high;
    dword reserved_high;

} interrupts_descriptor_t;

typedef interrupts_descriptor_t interrupts_idt_t[INTERRUPTS_IDT_SIZE];

interrupts_idt_t IDT;

static inline void interrupts_descriptor_init(interrupts_descriptor_t* descriptor, size_t interrupt_number);

static inline void interrupts_descriptor_init(interrupts_descriptor_t* descriptor, size_t interrupt_number)
{
    qword offset = (qword) (void*) isr_wrappers[interrupt_number];
    word offset_low = offset & 0xFFFF;
    word offset_middle = (offset >> 16) & 0xFFFF;
    dword offset_high = (offset >> 32) & 0xFFFFFFFF;
    descriptor->offset_low = offset_low;
    descriptor->offset_middle = offset_middle;
    descriptor->offset_high = offset_high;

    descriptor->segment_selector = INTERRUPTS_DESCRIPTOR_SEGMENT_SELECTOR;
    descriptor->ist_plus_reserved_low = (INTERRUPTS_DESCRIPTOR_RESERVED << 6) | INTERRUPTS_DESCRIPTOR_IST;
    descriptor->gate_type_plus_zero_plus_dpl_plus_p = (INTERRUPTS_DESCRIPTOR_PRESENT << 7)
                                                    | (INTERRUPTS_DESCRIPTOR_DPL << 5)
                                                    | (0 << 4)
                                                    | INTERRUPTS_DESCRIPTOR_GATE_TYPE_INTERRUPT;
    descriptor->reserved_high = 0;
}

void interrupts_setup(void)
{
    for (size_t i = 0; i < INTERRUPTS_IDT_SIZE; ++i)
    {
        interrupts_descriptor_init(&IDT[i], i);
    }
}
