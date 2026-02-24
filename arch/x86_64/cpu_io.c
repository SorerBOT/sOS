#include <cpu_io.h>
#include <types.h>

#define CPU_IO_WAIT_PORT 0x80
#define CPU_IO_WAIT_PORT 0x80

static inline void outb(word port, byte value);
static inline byte inb(word port);
static inline void wait();

static inline void outb(word port, byte value)
{
    __asm__ volatile ( "outb %0, %1" : : "a"(value), "Nd"(port) );
}

static inline byte inb(word port)
{
    byte value;
    __asm__ volatile ( "inb %w1, %b0"
            : "=a"(value)
            : "Nd"(port)
            : "memory");

    return value;
}

static inline void wait()
{
    outb(CPU_IO_WAIT_PORT, 0);
}

void cpu_io_write_byte(word port, byte value)
{
    outb(port, value);
}

byte cpu_io_read_byte(word port)
{
    return inb(port);
}

void cpu_io_wait(void)
{
    wait();
}
