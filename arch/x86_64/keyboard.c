#include <keyboard.h>
#include <ps2_keyboard_driver.h>


void keyboard_read_char(char* c)
{
    ps2_keyboard_driver_read_char(c);
}
