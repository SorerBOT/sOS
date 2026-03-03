#ifndef PS2_KEYBOARD_DRIVER_H
#define PS2_KEYBOARD_DRIVER_H

#include <types.h>

typedef struct
{
    word data_port;
} ps2_keyboard_driver_settings_t;

void ps2_keyboard_driver_init(const ps2_keyboard_driver_settings_t* settings);

/*
 * this function is only responsible for
 * reading a complete scancode from the
 * keyboard device controller, translating
 * it into a keycode, and calling the
 * generic driver's (keyboard_driver) functions
 * to record the event. It does not manage its
 * own ring_buffer, and the kernel does not
 * interact with it.
 */
void ps2_keyboard_driver_read_and_handle_scancode(void);



#endif /* PS2_KEYBOARD_DRIVER_H */
