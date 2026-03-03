#include <keyboard_driver.h>
#include <ring_buffer.h>
#include <string.h>

#define KEYBOARD_BUFFER_SIZE 256

static bool keycode_states[KEYBOARD_KEYCODE_COUNT] = { false };
static keyboard_event_t keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static ring_buffer_t keyboard_ring_buffer =
{
    .buffer = (byte*) &keyboard_buffer[0],
    .head = 0,
    .tail = 0,
    .size = KEYBOARD_BUFFER_SIZE
};

void keyboard_driver_record_event(keyboard_event_t event)
{
    byte* event_bytes = (byte*) &event;
    ring_buffer_try_write(&keyboard_ring_buffer,
            event_bytes,
            sizeof(keyboard_event_t));

    keycode_states[event.keycode] = (event.type == KEYBOARD_PRESSED);
}

bool keyboard_driver_get_key_state(keyboard_keycode_t keycode)
{
    return keycode_states[keycode];
}


void keyboard_driver_get_all_key_states(bool _keycode_states[KEYBOARD_KEYCODE_COUNT])
{
    memcpy(_keycode_states, keycode_states, KEYBOARD_KEYCODE_COUNT);
}

void keyboard_driver_read_char(char* c)
{
    while ( ring_buffer_read(&keyboard_ring_buffer, (byte*)c, sizeof(*c)) == 0 )
    {
        __asm__ volatile("hlt");
    }
}
