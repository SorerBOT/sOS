#include <math_extended.h>

/*
 * I read about this algorithm in Standford's Graphics Book:
 * https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
 */
qword math_extended_round_up_to_power_of_two(qword x)
{
    x += (x == 0);
    --x;

    math_extended_right_smear(x);

    ++x;
    return x;
}

/*
 * This was inspired by an algorithm taken from Hacker's Delight
 */
qword math_extended_round_up_to_log_two(qword x)
{
    x += (x == 0);
    --x;

    math_extended_right_smear(x);
    return math_extended_popcount(x);
}

qword math_extended_right_smear(qword x)
{
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;

    return x;
}

/*
 * I copied this from https://nimrod.blog/posts/algorithms-behind-popcount/
 * I'm pretty sure he copied it from GCC's implementation of __builtin_popcountll
 * for when the CPU does not support the native popcount instruction.
 */
qword math_extended_popcount(qword x)
{
    x = x - ((x >> 1) & 0x5555555555555555);
    x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
    x = (x + (x >> 4)) & 0xF0F0F0F0F0F0F0F;
    return x;
}
