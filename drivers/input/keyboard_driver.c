#include <keyboard_driver.h>
#include <ring_buffer.h>

#define KEYBOARD_BUFFER_SIZE 256

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
}

void keyboard_driver_read_char(char* c)
{
    while ( ring_buffer_read(&keyboard_ring_buffer, (byte*)c, sizeof(*c)) == 0 )
    {
        __asm__ volatile("hlt");
    }
}
