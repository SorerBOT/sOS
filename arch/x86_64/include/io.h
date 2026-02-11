#ifndef IO_H
#define IO_H

#include <types.h>

static inline void outb(word port, byte value)
{
    __asm__ volatile ( "outb %0, %1" : : "a"(value), "Nd"(port) );
}

#endif /* IO_H */
