#include <interrupts.h>
#include <types.h>
#include "include/idt.h"

void interrupts_setup(void)
{
    idt_setup();
}
