#ifndef MATH_H
#define MATH_H

#include <stdint.h>

typedef union
{
    uint64_t all;
    struct {
        uint32_t low;
        uint32_t high;
    } s;
} udwords;

int64_t __divdi3(int64_t a, int64_t b);
int64_t __moddi3(int64_t a, int64_t b);
uint64_t __udivmoddi4(uint64_t a, uint64_t b, uint64_t *rem);

#endif
