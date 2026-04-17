#ifndef CPU_IO_H
#define CPU_IO_H

#include <types.h>

byte cpu_io_read_byte(word port);
word cpu_io_read_word(word port);
dword cpu_io_read_dword(word port);

void cpu_io_write_byte(word port, byte value);
void cpu_io_write_word(word port, word value);
void cpu_io_write_dword(word port, dword value);

void cpu_io_wait(void);

#endif /* CPU_IO_H */
