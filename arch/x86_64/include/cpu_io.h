#ifndef CPU_IO_H
#define CPU_IO_H

#include <types.h>

#define CPU_IO_WAIT_PORT 0x80

static inline void cpu_io_outb(word port, byte value);
static inline byte cpu_io_inb(word port);
static inline void cpu_io_wait();


static inline void cpu_io_outb(word port, byte value)
{
    __asm__ volatile ( "outb %0, %1" : : "a"(value), "Nd"(port) );
}

static inline byte cpu_io_inb(word port)
{
    byte value;
    __asm__ volatile ( "inb %w1, %b0"
            : "=a"(value)
            : "Nd"(port)
            : "memory");

    return value;
}

static inline void cpu_io_wait()
{
    cpu_io_outb(CPU_IO_WAIT_PORT, 0);
}

#endif /* CPU_IO_H */
