#ifndef CPU_IO_H
#define CPU_IO_H

#include <types.h>

static inline void cpu_io_outb(word port, byte value)
{
    __asm__ volatile ( "outb %0, %1" : : "a"(value), "Nd"(port) );
}

#endif /* CPU_IO_H */
