#include <tty.h>
#include <types.h>
#include <keyboard_types.h>
#include <keyboard_driver.h>
#include <console_output.h>

#define TTY_BUFFER_SIZE 256

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

static char* handle_unicode(char* line_buffer, size_t buffer_size,
        const keyboard_unit_t* unit, const char* end_of_buffer,
        const char* start_of_buffer)
{
    *line_buffer = (char) unit->data.character;

    console_output_printf("%c", *line_buffer);
    console_output_flush();

    ++line_buffer;

    return line_buffer;
}

static char* handle_action_backspace(char* line_buffer, size_t buffer_size,
        const keyboard_unit_t* unit, const char* end_of_buffer,
        const char* start_of_buffer)
{
    if  ( line_buffer > start_of_buffer )
    {
        console_output_backspace();
        console_output_flush();
        --line_buffer;
    }

    return line_buffer;
}

static char* handle_action_backspace_word(char* line_buffer, size_t buffer_size,
        const keyboard_unit_t* unit, const char* end_of_buffer,
        const char* start_of_buffer)
{
    for (; line_buffer > start_of_buffer;)
    {
        console_output_backspace();
        console_output_flush();
        --line_buffer;
        if  ( *line_buffer == ' ' )
        {
            break;
        }
    }

    return line_buffer;
}

static char* handle_action_backspace_line(char* line_buffer, size_t buffer_size,
        const keyboard_unit_t* unit, const char* end_of_buffer,
        const char* start_of_buffer)
{
    for (; line_buffer > start_of_buffer; --line_buffer)
    {
        console_output_backspace();
        console_output_flush();
    }

    return line_buffer;
}

static char* handle_unit(char* line_buffer, size_t buffer_size,
        const keyboard_unit_t* unit, const char* end_of_buffer,
        const char* start_of_buffer)
{
    if ( unit->unit_type == KEYBOARD_UNIT_UNICODE )
    {
        return handle_unicode(line_buffer, buffer_size, unit,
                end_of_buffer, start_of_buffer);
    }
    else if ( unit->unit_type == KEYBOARD_UNIT_ACTION )
    {
        if ( unit->data.action.key == KEYBOARD_KEYCODE_BACKSPACE )
        {
            keyboard_modifiers_state_t control_l_or_option_l_mask = KEYBOARD_MODIFIERS_CONTROL_L | KEYBOARD_MODIFIERS_ALT_L;
            keyboard_modifiers_state_t super_mask = KEYBOARD_MODIFIERS_SUPER_ANY;

            if ( unit->data.action.modifiers_state & control_l_or_option_l_mask )
            {
                return handle_action_backspace_word(line_buffer, buffer_size, unit,
                        end_of_buffer, start_of_buffer);
            }
            else if ( unit->data.action.modifiers_state & super_mask )
            {
                return handle_action_backspace_line(line_buffer, buffer_size, unit,
                        end_of_buffer, start_of_buffer);
            }
            else
            {
                return handle_action_backspace(line_buffer, buffer_size, unit,
                        end_of_buffer, start_of_buffer);
            }
        }
    }

    return line_buffer;
}

errors_t tty_read_line(char* line_buffer, size_t buffer_size)
{
    const char* end_of_buffer = line_buffer + buffer_size;
    const char* start_of_buffer = line_buffer;

    keyboard_unit_t unit;

    for (; line_buffer < end_of_buffer - 1; )
    {
        keyboard_driver_consume_unit(&unit);
        line_buffer = handle_unit(line_buffer, buffer_size, &unit,
                end_of_buffer, start_of_buffer);

        if ( *( line_buffer - 1 ) == '\n' )
        {
            *( line_buffer - 1 ) = '\0';
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


