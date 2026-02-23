#include <interrupts.h>
#include <types.h>
#include "include/idt.h"

extern void interrupts_enable(void);

void interrupts_setup(void)
{
    idt_setup();
    interrupts_enable();
}
