#include <cpu_io.h>
#include <types.h>

#define CPU_IO_WAIT_PORT 0x80
#define CPU_IO_WAIT_PORT 0x80

static inline byte inb(word port);
static inline word inw(word port);
static inline dword inl(word port);

static inline void outb(word port, byte value);
static inline void outw(word port, word value);
static inline void outl(word port, dword value);

static inline void wait();

static inline byte inb(word port)
{
    byte value;
    __asm__ volatile ( "inb %w1, %b0"
            : "=a"(value)
            : "Nd"(port)
            : "memory");

    return value;
}

static inline word inw(word port)
{
    word value;
    __asm__ volatile ( "inw %w1, %w0"
            : "=a"(value)
            : "Nd"(port)
            : "memory");

    return value;
}

static inline dword inl(word port)
{
    dword value;
    __asm__ volatile ( "inl %w1, %k0"
            : "=a"(value)
            : "Nd"(port)
            : "memory");

    return value;
}

static inline void outb(word port, byte value)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(value), "Nd"(port) );
}

static inline void outw(word port, word value)
{
    __asm__ volatile ( "outw %w0, %w1" : : "a"(value), "Nd"(port) );
}

static inline void outl(word port, dword value)
{
    __asm__ volatile ( "outl %k0, %w1" : : "a"(value), "Nd"(port) );
}


static inline void wait()
{
    outb(CPU_IO_WAIT_PORT, 0);
}

byte cpu_io_read_byte(word port)
{
    return inb(port);
}

word cpu_io_read_word(word port)
{
    return inw(port);
}

dword cpu_io_read_dword(word port)
{
    return inl(port);
}

void cpu_io_write_byte(word port, byte value)
{
    outb(port, value);
}

void cpu_io_write_word(word port, word value)
{
    outw(port, value);
}

void cpu_io_write_dword(word port, dword value)
{
    outl(port, value);
}

void cpu_io_wait(void)
{
    wait();
}
