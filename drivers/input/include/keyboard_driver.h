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

void keyboard_driver_record_event(keyboard_event_t event);



/* ----- KERNEL FACING -----*/
bool keyboard_driver_get_key_state(keyboard_keycode_t keycode);
void keyboard_driver_get_all_key_states(bool _keycode_states[KEYBOARD_KEYCODE_COUNT]);

void keyboard_driver_read_event(keyboard_event_t* _event);
errors_t keyboard_driver_try_read_event(keyboard_event_t* _event);

void keyboard_driver_read_unit(char* c);



#endif /* KEYBOARD_DRIVER_H */
