#include <interrupts.h>
#include <types.h>

#define INTERRUPTS_IDT_SIZE 256

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

static inline void interrupts_descriptor_init(interrupts_descriptor_t* descriptor);
static inline word interrupts_get_segment_selector();


static inline word interrupts_get_segment_selector()
{
    word selector = 0;
    selector |= 0;       // RPL
    selector |= 0 << 2;  // TI
    selector |= 2 << 3;  // Index

    return selector;
}

static inline void interrupts_descriptor_init(interrupts_descriptor_t* descriptor)
{
    descriptor->segment_selector = interrupts_get_segment_selector();
}

void interrupts_setup(void)
{
    
}
