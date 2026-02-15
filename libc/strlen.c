#include <string.h>

size_t strlen(const char *s)
{
    size_t len = 0;

    for (; *s != '\0'; ++len, ++s);

    return len;
}
