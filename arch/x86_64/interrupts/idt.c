#include "include/idt.h"
#include "include/isr_wrappers.h"

extern void idt_load(idt_header_t* idt);
static inline void idt_descriptor_init(idt_descriptor_t* descriptor, size_t interrupt_number);
static inline void idt_header_init();

idt_t idt;
idt_header_t idt_header;

static inline void idt_descriptor_init(idt_descriptor_t* descriptor, size_t interrupt_number)
{
    qword offset = (qword) (void*) isr_wrappers[interrupt_number];
    word offset_low = offset & 0xFFFF;
    word offset_middle = (offset >> 16) & 0xFFFF;
    dword offset_high = (offset >> 32) & 0xFFFFFFFF;
    descriptor->offset_low = offset_low;
    descriptor->offset_middle = offset_middle;
    descriptor->offset_high = offset_high;

    descriptor->segment_selector = IDT_SEGMENT_SELECTOR;
    descriptor->ist_plus_reserved_low = (IDT_RESERVED << 6) | IDT_IST;
    descriptor->gate_type_plus_zero_plus_dpl_plus_p = (IDT_PRESENT << 7)
                                                    | (IDT_DPL << 5)
                                                    | (0 << 4)
                                                    | IDT_GATE_TYPE_INTERRUPT;
    descriptor->reserved_high = 0;
}

static inline void idt_header_init()
{
    idt_header.descriptors = &idt[0];
    idt_header.size = IDT_SIZE * sizeof(idt_descriptor_t) - 1;
}


void idt_setup(void)
{
    for (size_t i = 0; i < IDT_SIZE; ++i)
    {
        idt_descriptor_init(&idt[i], i);
    }

    idt_header_init();

    idt_load(&idt_header);
}
