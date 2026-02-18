#include <interrupts.h>
#include <stddef.h>
#include <types.h>

#define INTERRUPTS_IDT_SIZE 256

#define INTERRUPTS_SEGMENT_SELECTOR_RPL 0
#define INTERRUPTS_SEGMENT_SELECTOR_TI 0
#define INTERRUPTS_SEGMENT_SELECTOR_CODE_SEGMENT_IDX 2
#define INTERRUPTS_SEGMENT_SELECTOR ((INTERRUPTS_SEGMENT_SELECTOR_CODE_SEGMENT_IDX << 3) | (INTERRUPTS_SEGMENT_SELECTOR_TI << 2) | INTERRUPTS_SEGMENT_SELECTOR_RPL)


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

static inline void interrupts_descriptor_init(interrupts_descriptor_t* descriptor);
static inline word interrupts_get_segment_selector();


static inline word interrupts_get_segment_selector()
{
    word selector = 0;
    selector |= 0;       // RPL
    selector |= 0 << 2;  // TI
    selector |= INTERRUPTS_GDT_KERNEL_CODE_IDX << 3;  // Index

    return selector;
}

static inline void interrupts_descriptor_init(interrupts_descriptor_t* descriptor)
{
}

void interrupts_setup(void)
{
    word selector = interrupts_get_segment_selector();

    for (size_t i = 0; i < INTERRUPTS_IDT_SIZE; ++i)
    {
        interrupts_descriptor_init(&IDT[i]);
    }

}
