#ifndef IDT_H
#define IDT_H

#include <types.h>

#define IDT_SIZE 256
#define IDT_SEGMENT_SELECTOR_RPL 0
#define IDT_SEGMENT_SELECTOR_TI 0
#define IDT_SEGMENT_SELECTOR_CODE_SEGMENT_IDX 2
#define IDT_SEGMENT_SELECTOR ((IDT_SEGMENT_SELECTOR_CODE_SEGMENT_IDX << 3) | (IDT_SEGMENT_SELECTOR_TI << 2) | IDT_SEGMENT_SELECTOR_RPL)
#define IDT_IST 0b00
#define IDT_RESERVED 0b000000
#define IDT_GATE_TYPE_INTERRUPT 0xE
#define IDT_GATE_TYPE_TRAP 0xF
#define IDT_DPL 0b00
#define IDT_PRESENT 0b1

typedef struct
{
    word offset_low;
    word segment_selector;
    byte ist_plus_reserved_low;
    byte gate_type_plus_zero_plus_dpl_plus_p;
    word offset_middle;
    dword offset_high;
    dword reserved_high;
} __attribute__((packed)) idt_descriptor_t;

typedef struct
{
    word size;
    idt_descriptor_t* descriptors;
} __attribute__((packed)) idt_header_t;

typedef idt_descriptor_t idt_t[IDT_SIZE];

void idt_setup(void);

#endif /* IDT_H */
