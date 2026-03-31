#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define KiB 1024
#define MiB (1024 * KiB)
#define GiB (1024 * MiB)
#define TiB (1024 * GiB)

typedef enum
{
    ERRORS_NONE,
    ERRORS_WRITE_NOT_ENOUGH_SPACE,
    ERRORS_NOT_ENOUGH_DATA,
    ERRORS_INVALID_PARAMETERS,
    ERRORS_FAILED,
} errors_t;

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;
typedef uint64_t qword;

typedef uint32_t utf32_t;

#endif /* TYPES_H */
