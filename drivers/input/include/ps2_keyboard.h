#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H

#include <types.h>

typedef struct
{
    word data_address;
} ps2_keyboard_settings_t;

void ps2_keyboard_init(const ps2_keyboard_settings_t* settings);

/* HARDWARE FACING */
void ps2_keyboard_write_char();


/* KERNEL FACING */





#endif /* PS2_KEYBOARD_H */
