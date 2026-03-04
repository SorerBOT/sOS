#include <keyboard.h>
#include <keyboard_driver.h>

void keyboard_read_char(char* c)
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
