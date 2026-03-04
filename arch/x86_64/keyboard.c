#include <keyboard.h>
#include <keyboard_driver.h>



#include <console_io.h>


void keyboard_read_char(char* c)
{
    keyboard_unit_t unit;
    for (;;)
    {
        keyboard_driver_consume_unit(&unit);

        console_io_printf("Consumed keyboard unit.\n");

        if ( unit.unit_type == KEYBOARD_UNIT_ACTION )
        {
            console_io_printf("Modifiers: %08b\n", unit.data.action.modifiers_state);
            console_io_printf("Key: %02x\n", unit.data.action.key);
            continue;
        }
        else
        {
            console_io_printf("Character: %c\n", unit.data.character);
        }

        if ( unit.unit_type == KEYBOARD_UNIT_UNICODE )
        {
            *c = (char) unit.data.character;
            break;
        }
    }
}
