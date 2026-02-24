#include "string.h"
#include <ps2_keyboard.h>

static ps2_keyboard_settings_t settings;

void ps2_keyboard_init(const ps2_keyboard_settings_t* _settings)
{
    settings = *_settings;
}


