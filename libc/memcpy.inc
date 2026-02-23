#include <types.h>

MEMCPY_RETURN_TYPE MEMCPY_VERSION_NAME(
        MEMCPY_DST_QUALIFIER_BEFORE void* MEMCPY_DST_QUALIFIER_AFTER dst,
        MEMCPY_SRC_QUALIFIER_BEFORE void* MEMCPY_SRC_QUALIFIER_AFTER src,
        size_t n)
{
    MEMCPY_DST_QUALIFIER_BEFORE byte* MEMCPY_DST_QUALIFIER_AFTER dst_byte = dst;
    MEMCPY_SRC_QUALIFIER_BEFORE byte* MEMCPY_SRC_QUALIFIER_AFTER src_byte = src;

    if ( (((uintptr_t) dst) & 7) == (((uintptr_t) src) & 7) )
    {
        // aligning the pointers
        for (; (n > 0) && (((uintptr_t) dst_byte) & 7) != 0; --n)
        {
            *(dst_byte++) = *(src_byte++);
        }

        // copying in 8 byte chunks
        MEMCPY_DST_QUALIFIER_BEFORE qword* MEMCPY_DST_QUALIFIER_AFTER dst_qword =
            (MEMCPY_DST_QUALIFIER_BEFORE qword*) dst_byte;

        MEMCPY_SRC_QUALIFIER_BEFORE qword* MEMCPY_SRC_QUALIFIER_AFTER src_qword =
            (MEMCPY_SRC_QUALIFIER_BEFORE qword*) src_byte;

        for (; n >= 8; n -= 8)
        {
            *(dst_qword++) = *(src_qword++);
        }

        // copying left-overs
        if (n > 0)
        {
            dst_byte = (MEMCPY_DST_QUALIFIER_BEFORE byte* MEMCPY_DST_QUALIFIER_AFTER) dst_qword;
            src_byte = (MEMCPY_SRC_QUALIFIER_BEFORE byte* MEMCPY_SRC_QUALIFIER_AFTER) src_qword;

            for (; n > 0; --n)
            {
                *(dst_byte++) = *(src_byte++);
            }
        }
    }
    else
    {
        for (; n > 0; --n)
        {
            *(dst_byte++) = *(src_byte++);
        }
    }

    return dst;
}
