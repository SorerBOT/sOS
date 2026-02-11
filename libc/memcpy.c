#include <types.h>
#include <string.h>

void* memcpy(void *restrict dst, const void *restrict src, size_t n)
{
    byte* restrict dst_byte = dst;
    const byte* restrict src_byte = src;

    while (n && (uintptr_t) dst_byte % 8 != 0)
    {
        *(dst_byte++) = *(src_byte++);
        --n;
    }

    qword* restrict dst_qword = (qword*) dst_byte;
    const qword* restrict src_qword = (const qword*) src_byte;

    while (n >= 8)
    {
        *(dst_qword++) = *(src_qword++);
        n -= 8;
    }

    if (n > 0)
    {
        dst_byte = (byte*) dst_qword;
        src_byte = (byte*) src_qword;

        while (n--)
        {
            *(dst_byte++) = *(src_byte++);
        }
    }

    return dst;
}
