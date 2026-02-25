#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <libc_partials/include/types.h>

void* memset_word(void *address, int value_int, size_t len_in_words);
void* memset(void *address, int value_int, size_t len_in_bytes);
volatile void* memcpy_from_volatile_to_volatile(volatile void* dst, const volatile void* src, size_t size);

#endif /* STRING_H */
