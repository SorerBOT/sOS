#ifndef PS2_KEYBOARD_DRIVER_H
#define PS2_KEYBOARD_DRIVER_H

#include <types.h>

typedef struct
{
    word data_port;
} ps2_keyboard_driver_settings_t;

void ps2_keyboard_driver_init(const ps2_keyboard_driver_settings_t* settings);

/* HARDWARE FACING */
void ps2_keyboard_driver_read_and_handle_scancode(void);


/* KERNEL FACING */
void ps2_keyboard_driver_read_char(char* c);




#endif /* PS2_KEYBOARD_DRIVER_H */
