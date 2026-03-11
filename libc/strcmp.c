#include <string.h>

int strcmp(const char *first, const char* second)
{
    for (; *first != '\0' && *second != '\0'; ++first, ++second)
    {
        if ( *first != *second )
        {
            break;
        }
    }

    return *first - *second;
}
