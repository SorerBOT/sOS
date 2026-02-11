#include <string.h>

void* memset(void *address, int value_int, size_t len_in_bytes)
{
    byte value = (byte) value_int;
    byte* data = address;
    for (size_t i = 0; i < len_in_bytes; ++i)
    {
        data[i] = value;
    }

    return address;
}
