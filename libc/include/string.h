#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <types.h>

void* memset_word(void *address, int value_int, size_t len_in_words);
void* memset(void *address, int value_int, size_t len_in_bytes);
void* memcpy(void *restrict dst, const void *restrict src, size_t n);
volatile void* memcpy_to_volatile(volatile void *dst, const void *restrict src, size_t n);
void* memcpy_from_volatile(void *restrict dst, const volatile void * src, size_t n);

size_t strlen(const char *s);


#endif /* STRING_H */
