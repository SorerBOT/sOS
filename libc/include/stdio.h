#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdarg.h>

int vsnprintf(char* restrict str, size_t size, const char* restrict format, va_list ap);

#endif /* STDIO_H */
