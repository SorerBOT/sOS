#ifndef CONSOLE_OUTPUT_H
#define CONSOLE_OUTPUT_H

#include <stddef.h>
#include <stdbool.h>

typedef enum
{
    CONSOLE_OUTPUT_FAILURE = 0,
    CONSOLE_OUTPUT_SUCCESS = 1
} console_output_report_status_t;

typedef struct
{
    size_t initial_line;
    bool should_copy_existing_buffer;
} console_output_init_settings_t;

void console_output_report(const char* restrict message, console_output_report_status_t status);
void console_output_printf(const char* restrict format, ...);
void console_output_clear();
void console_output_print_blue_screen(const char* restrict format, ...);
void console_output_flush();
void console_output_init(const console_output_init_settings_t* settings);
void console_output_backspace(void);

#endif /* CONSOLE_OUTPUT_H */
