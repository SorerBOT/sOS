#include <interrupts.h>
#include <types.h>
#include "include/idt.h"
#include "include/pic.h"

extern void interrupts_enable(void);

void interrupts_setup(void)
{
    idt_setup();
    pic_remap();
    interrupts_enable();
}
