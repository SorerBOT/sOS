/* Im sorta using two namespaces, because KEYBOARD will soon be separated from ps2_keyboard_driver.
 * keyboard will handle a general keypress, without the specifics of how to read the data when using ps2.
 * don't freak out.
 */

#include <ps2_keyboard_driver.h>
#include <cpu_io.h>
#include <keyboard_driver.h>
#include <scancode_1.h>
#include <types.h>

static errors_t parse_scancode(byte scancode, keyboard_event_t* event);

static ps2_keyboard_driver_settings_t settings;

static errors_t parse_scancode(byte scancode, keyboard_event_t* event)
{
    keyboard_keycode_t keycode;
    if ( scancode < SCANCODE_1_EXTENDED_OFFSET )
    {
        event->type = (scancode < SCANCODE_1_SINGLE_BYTE_MAKE_SCANCODES_COUNT)
            ? KEYBOARD_PRESSED
            : KEYBOARD_RELEASED;
        keycode = map_scancode_to_keycode[scancode];
        return ERRORS_NONE;
    }

    return ERRORS_FAILED;
}

void ps2_keyboard_driver_init(const ps2_keyboard_driver_settings_t* _settings)
{
    settings = *_settings;
}

void ps2_keyboard_driver_read_and_handle_scancode(void)
{
    byte scancode = cpu_io_read_byte(settings.data_port);
    keyboard_event_t event;

    if ( parse_scancode(scancode, &event) == ERRORS_NONE )
    {
        keyboard_driver_record_event(event);
    }
}
