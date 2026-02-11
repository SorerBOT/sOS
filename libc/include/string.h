#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <types.h>

void* memset(void *address, int value_int, size_t len);
void* memcpy(void *restrict dst, const void *restrict src, size_t n);


#endif /* STRING_H */
