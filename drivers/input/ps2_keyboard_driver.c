/* Im sorta using two namespaces, because KEYBOARD will soon be separated from ps2_keyboard_driver.
 * keyboard will handle a general keypress, without the specifics of how to read the data when using ps2.
 * don't freak out.
 */

#include "keyboard_types.h"
#include <ps2_keyboard_driver.h>
#include <cpu_io.h>
#include <console_io.h>
#include <ring_buffer.h>
#include <keyboard_driver.h>
#include <scancode_1.h>


#define KEYBOARD_BUFFER_SIZE 256

static void parse_scancode(byte scancode, keyboard_event_t* event);

static ps2_keyboard_driver_settings_t settings;
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static ring_buffer_t keyboard_ring_buffer;

static void parse_scancode(byte scancode, keyboard_event_t* event)
{
    keyboard_keycode_t keycode;
    if ( scancode < SCANCODE_1_EXTENDED_OFFSET )
    {
        event->type = (scancode < SCANCODE_1_SINGLE_BYTE_MAKE_SCANCODES_COUNT)
            ? KEYBOARD_PRESSED
            : KEYBOARD_RELEASED;
        keycode = map_scancode_to_keycode[scancode];
    }

    else
    {
        event->type = KEYBOARD_INVALID;
        event->keycode = KEYBOARD_KEYCODE_INVALID;
    }
}

void ps2_keyboard_driver_init(const ps2_keyboard_driver_settings_t* _settings)
{
    settings = *_settings;

    keyboard_ring_buffer = (ring_buffer_t)
    {
        .buffer = (byte*) &keyboard_buffer[0],
        .head = 0,
        .tail = 0,
        .size = KEYBOARD_BUFFER_SIZE
    };
}

void ps2_keyboard_driver_read_and_handle_scancode(void)
{
    byte scancode = cpu_io_read_byte(settings.data_port);
    keyboard_event_t event;
    parse_scancode(scancode, &event);

    if ( event->event_type == KEYBOARD_RELEASED )
    {
        //ring_buffer_try_write(&keyboard_ring_buffer, (byte*)&event->key_ascii, sizeof(event->key_ascii));
    }
}

void ps2_keyboard_driver_read_char(char* c)
{
    while ( ring_buffer_read(&keyboard_ring_buffer, (byte*)c, sizeof(*c)) == 0 )
    {
        __asm__ volatile("hlt");
    }
}
