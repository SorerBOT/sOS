#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdbool.h>

typedef enum
{
    STDIO_SUCCESS,
    STDIO_FAILURE
} stdio_report_status_t;

typedef struct
{
    size_t initial_line;
    bool should_copy_existing_buffer;
} stdio_init_settings_t;

void report(const char* restrict message, stdio_report_status_t status);
void printf(const char* restrict format, ...);
void clear();
void print_blue_screen(const char* restrict format, ...);
void flush();
void stdio_init(const stdio_init_settings_t* settings);

#endif /* STDIO_H */
