#include <math_extended.h>

/*
 * I read about this algorithm in Standford's Graphics Book:
 * https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
 */
qword math_extended_round_up_to_power_of_two(qword x)
{
    x += (x == 0);
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    ++x;
    return x;
}
