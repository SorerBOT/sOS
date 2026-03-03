#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H

#include <keyboard_types.h>

/*
 * ----- HARDWARE FACING -----
 * the hardware facing part of this driver is
 * responsible for allowing the ps2/USB keyboard
 * drivers to record the key press / key releases
 * that they generate as described in the keyboard
 * spec
 */

void keyboard_driver_record_event(void);



/* KERNEL FACING */
void ps2_keyboard_driver_read_char(char* c);



#endif /* KEYBOARD_DRIVER_H */
