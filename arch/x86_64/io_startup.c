#include <io_setup.h>
#include <console_io.h>
#include <ps2_keyboard.h>

static inline void inputs_setup(void);
static inline void outputs_setup(void);

static inline void inputs_setup(void)
{
    ps2_keyboard_settings_t settings =
    {
        .data_port = 0x60
    };
    ps2_keyboard_init(&settings);
}

static inline void outputs_setup(void)
{
    console_io_init_settings_t settings =
    {
        .initial_line = 19,
            .should_copy_existing_buffer = true
    };

    console_io_init(&settings);
}

void io_setup()
{
    inputs_setup();
    outputs_setup();
}
