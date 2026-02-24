#include <io_startup.h>
#include <console_io.h>
#include <ps2_keyboard.h>

static inline void inputs_startup(void);
static inline void outputs_startup(void);

static inline void inputs_startup(void)
{
    ps2_keyboard_settings_t settings =
    {
        .data_address = 0x60
    };
}

static inline void outputs_startup(void)
{
    console_io_init_settings_t settings =
    {
        .initial_line = 19,
            .should_copy_existing_buffer = true
    };

    console_io_init(&settings);
}

void io_startup()
{
    inputs_startup();
    outputs_startup();
}
