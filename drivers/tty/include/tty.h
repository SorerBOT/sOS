#ifndef TTY_H
#define TTY_H

#include <types.h>

errors_t tty_read_line(char* line_buffer, size_t buffer_size);
void tty_print_line(const char* line);

#endif /* TTY_H */
