#include <keyboard.h>
#include <ps2_keyboard.h>


void keyboard_read_char(char* c)
{
    ps2_keyboard_read_char(c);
}
