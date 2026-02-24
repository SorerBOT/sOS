#ifndef CPU_IO_H
#define CPU_IO_H

#include <types.h>

void cpu_io_write_byte(word port, byte value);
byte cpu_io_read_byte(word port);
void cpu_io_wait(void);

#endif /* CPU_IO_H */
