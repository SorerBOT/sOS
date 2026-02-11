#include <string.h>

void* memset_word(void *address, int value_int, size_t len_in_words)
{
    word value = (word) value_int;

    qword* address_qword = (qword*) address;
    if (len_in_words >= 4)
    {
        qword value_qword = (qword) value;
        qword concatenated_value = (value_qword << 48) | (value_qword << 32) | (value_qword << 16) | value_qword;
        for (; len_in_words >= 4; len_in_words -= 4, ++address_qword)
        {
            *address_qword = concatenated_value;
        }
    }

    if (len_in_words > 0)
    {
        word* address_word = (word*) address_qword;
        for (; len_in_words > 0; --len_in_words, ++address_word)
        {
            *address_word = value;
        }
    }

    return address;
}
