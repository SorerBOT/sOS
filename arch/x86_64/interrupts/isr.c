#include "include/isr.h"
#include <console_io.h>

void isr_handler(isr_args_t* args)
{
    if ( args->isr_number == ISR_BREAKPOINT )
    {
        console_io_print_blue_screen("Breakpoint on instruction %p reached.\n", args->instruction_pointer);
    }
}
