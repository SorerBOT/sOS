#include "include/keyboard_manager.h"
#include <keyboard_driver.h>

void keyboard_manager_set_layout(const keyboard_layout_t* layout)
{
    keyboard_driver_set_layout(layout);
}
