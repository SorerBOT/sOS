#include "types.h"
#include <tty.h>
#include <keyboard_types.h>
#include <keyboard_driver.h>
#include <console_output.h>

static inline void tty_read_char(char* c);

static inline void tty_read_char(char* c)
{
    keyboard_unit_t unit;
    for (;;)
    {
        keyboard_driver_consume_unit(&unit);

        if ( unit.unit_type == KEYBOARD_UNIT_UNICODE )
        {
            *c = (char) unit.data.character;
            break;
        }
    }
}

errors_t tty_read_line(char* line_buffer, size_t buffer_size)
{
    const char* end_of_buffer = line_buffer + buffer_size;
    for (; line_buffer < end_of_buffer - 1; ++line_buffer )
    {
        tty_read_char(line_buffer);
        console_output_printf("%c", *line_buffer);
        console_output_flush();
        if ( *line_buffer == '\n' )
        {
            ++line_buffer;
            *line_buffer = '\0';
            return ERRORS_NONE;
        }
    }

    *line_buffer = '\0';
    
    return ERRORS_WRITE_NOT_ENOUGH_SPACE;
}

void tty_print_line(const char* line)
{
    console_output_printf("%s\n", line);
}


