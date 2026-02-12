#ifndef CONSOLE_IO_H
#define CONSOLE_IO_H

#include <stddef.h>
#include <stdbool.h>

typedef enum
{
    CONSOLE_IO_SUCCESS,
    CONSOLE_IO_FAILURE
} console_io_report_status_t;

typedef struct
{
    size_t initial_line;
    bool should_copy_existing_buffer;
} console_io_init_settings_t;

void console_io_report(const char* restrict message, console_io_report_status_t status);
void console_io_printf(const char* restrict format, ...);
void console_io_clear();
void console_io_print_blue_screen(const char* restrict format, ...);
void console_io_flush();
void console_io_init(const console_io_init_settings_t* settings);

#endif /* CONSOLE_IO_H */
