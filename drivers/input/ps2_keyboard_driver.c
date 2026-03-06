#include <ps2_keyboard_driver.h>
#include <cpu_io.h>
#include <keyboard_driver.h>
#include <scancode_1.h>
#include <types.h>

typedef enum
{
    PS2_KEYBOARD_DRIVER_NORMAL,
    PS2_KEYBOARD_DRIVER_EXTENDED
} ps2_keyboard_driver_state_type_t;

static ps2_keyboard_driver_settings_t settings;
static ps2_keyboard_driver_state_type_t current_state = PS2_KEYBOARD_DRIVER_NORMAL;

static inline errors_t consume_scancode_normal(byte scancode, keyboard_event_t* event);
static inline errors_t consume_scancode_extended(byte scancode, keyboard_event_t* event);
static inline errors_t consume_scancode(byte scancode, keyboard_event_t* event);

static inline errors_t consume_scancode_normal(byte scancode, keyboard_event_t* event)
{
    if ( scancode < SCANCODE_1_EXTENDED_OFFSET )
    {
        event->type = (scancode < SCANCODE_1_NORMAL_MAKE_SCANCODES_COUNT)
            ? KEYBOARD_PRESSED
            : KEYBOARD_RELEASED;

        byte make_scancode = scancode & ~SCANCODE_1_NORMAL_MAKE_SCANCODES_COUNT;
        event->keycode = map_normal_scancode_to_keycode[make_scancode];

        return ERRORS_NONE;
    }

    else if ( scancode == SCANCODE_1_EXTENDED_OFFSET )
    {
        current_state = PS2_KEYBOARD_DRIVER_EXTENDED;
        return ERRORS_NOT_ENOUGH_DATA;
    }

    return ERRORS_INVALID_PARAMETERS;
}

static inline errors_t consume_scancode_extended(byte scancode, keyboard_event_t* event)
{
    current_state = PS2_KEYBOARD_DRIVER_NORMAL;

    if ( scancode <= SCANCODE_1_EXTENDED_MAX_SCANCODE )
    {
        event->type = (scancode < SCANCODE_1_EXTENDED_MAKE_SCANCODES_COUNT)
            ? KEYBOARD_PRESSED
            : KEYBOARD_RELEASED;

        byte make_scancode = scancode & ~SCANCODE_1_NORMAL_MAKE_SCANCODES_COUNT;
        event->keycode = map_normal_scancode_to_keycode[make_scancode];

        return ERRORS_NONE;
    }

    return ERRORS_INVALID_PARAMETERS;
}

static inline errors_t consume_scancode(byte scancode, keyboard_event_t* event)
{
    switch ( current_state )
    {
        case PS2_KEYBOARD_DRIVER_NORMAL:
            return consume_scancode_normal(scancode, event);
            break;

        case PS2_KEYBOARD_DRIVER_EXTENDED:
            return consume_scancode_extended(scancode, event);
            break;
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

    if ( consume_scancode(scancode, &event) == ERRORS_NONE )
    {
        keyboard_driver_record_event(event);
    }
}
