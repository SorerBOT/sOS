#include <keyboard_types.h>
#include <types.h>
#include <keyboard_driver.h>
#include <ring_buffer.h>
#include <string.h>

#define KEYBOARD_DRIVER_BUFFER_SIZE 256

static const keyboard_layout_t* keyboard_layout = NULL;
static keyboard_modifiers_state_t modifiers_state = 0;
static keyboard_modifiers_state_t mapping_keycode_to_modifier[KEYBOARD_KEYCODE_COUNT];
static bool keycode_states[KEYBOARD_KEYCODE_COUNT] = { false };
static keyboard_event_t keyboard_buffer[KEYBOARD_DRIVER_BUFFER_SIZE];
static ring_buffer_t keyboard_ring_buffer =
{
    .buffer = (byte*) &keyboard_buffer[0],
    .head = 0,
    .tail = 0,
    .size = KEYBOARD_DRIVER_BUFFER_SIZE
};

static inline bool is_modifier(keyboard_keycode_t keycode);
static inline bool is_toggled_modifier(keyboard_modifiers_type_t modifier);
static inline bool is_terminal_event(const keyboard_event_t* event);
static inline bool is_action_unit_because_modifiers(keyboard_modifiers_state_t modifiers_state);
static inline utf32_t transform_to_unicode(keyboard_modifiers_state_t modifiers_state, keyboard_keycode_t keycode);

static void build_unit(keyboard_unit_t* dst,
        const keyboard_event_t* current_event,
        keyboard_modifiers_state_t modifiers_state);

static inline bool is_modifier(keyboard_keycode_t keycode)
{
    return mapping_keycode_to_modifier[keycode] != KEYBOARD_MODIFIERS_NONE;
}

static inline bool is_toggled_modifier(keyboard_modifiers_type_t modifier)
{
    return ( modifier == KEYBOARD_MODIFIERS_CAPS_LOCK || modifier == KEYBOARD_MODIFIERS_NUM_LOCK );
}

static inline bool is_terminal_event(const keyboard_event_t* event)
{
    return !is_modifier(event->keycode) && event->type == KEYBOARD_PRESSED;
}

static inline bool is_action_unit_because_modifiers(keyboard_modifiers_state_t modifiers_state)
{
    return (
            modifiers_state & 
            (
             KEYBOARD_MODIFIERS_CONTROL_ANY |
             KEYBOARD_MODIFIERS_SUPER_ANY   |
             KEYBOARD_MODIFIERS_ALT_L
            )
           ) != 0;
}

static inline utf32_t transform_to_unicode(keyboard_modifiers_state_t modifiers_state, keyboard_keycode_t keycode)
{
    bool is_alted = modifiers_state & KEYBOARD_MODIFIERS_ALT_R;
    bool is_caps_lock = modifiers_state & KEYBOARD_MODIFIERS_CAPS_LOCK;
    bool is_keycode_modified_under_caps_lock = keyboard_layout->is_modified_under_caps_lock[keycode];
    bool is_shifted = is_caps_lock && is_keycode_modified_under_caps_lock;

    if ( modifiers_state & KEYBOARD_MODIFIERS_SHIFT_ANY )
    {
        is_shifted = !is_shifted;
    }

    if ( is_shifted && is_alted )
    {
        return keyboard_layout->map_shifted_alted[keycode];
    }

    if ( is_shifted )
    {
        return keyboard_layout->map_shifted[keycode];
    }

    if ( is_alted )
    {
        return keyboard_layout->map_alted[keycode];
    }

    return keyboard_layout->map_normal[keycode];
}

static void build_unit(keyboard_unit_t* dst,
        const keyboard_event_t* current_event,
        keyboard_modifiers_state_t modifiers_state)
{
    dst->event_type = current_event->type;

    if ( is_action_unit_because_modifiers(modifiers_state) )
    {
        dst->unit_type = KEYBOARD_UNIT_ACTION;
        dst->data.action = (keyboard_action_t)
        {
            .modifiers_state = modifiers_state,
            .key = current_event->keycode
        };
        return;
    }

    utf32_t character = transform_to_unicode(modifiers_state, current_event->keycode);
    if ( character != KEYBOARD_NON_RESOLVABLE_UNICODE )
    {
        dst->unit_type = KEYBOARD_UNIT_UNICODE;
        dst->data.character = character;
        return;
    }

    else
    {
        dst->unit_type = KEYBOARD_UNIT_ACTION;
        dst->data.action = (keyboard_action_t)
        {
            .modifiers_state = modifiers_state,
            .key = current_event->keycode
        };
        return;
    }
}

void keyboard_driver_record_event(keyboard_event_t event)
{
    byte* event_bytes = (byte*) &event;
    ring_buffer_try_write(&keyboard_ring_buffer,
            event_bytes,
            sizeof(keyboard_event_t));

    keycode_states[event.keycode] = (event.type == KEYBOARD_PRESSED);
}

bool keyboard_driver_get_key_state(keyboard_keycode_t keycode)
{
    return keycode_states[keycode];
}


void keyboard_driver_get_all_key_states(bool _keycode_states[KEYBOARD_KEYCODE_COUNT])
{
    memcpy(_keycode_states, keycode_states, KEYBOARD_KEYCODE_COUNT);
}

errors_t keyboard_driver_try_consume_event(keyboard_event_t* dst)
{
    errors_t error = ring_buffer_read(&keyboard_ring_buffer, (byte*) dst, sizeof(*dst));
    if ( error == ERRORS_NONE )
    {
        keyboard_modifiers_type_t modifier = mapping_keycode_to_modifier[dst->keycode];
        if ( modifier != KEYBOARD_MODIFIERS_NONE )
        {
            if ( !( is_toggled_modifier(modifier) && dst->type == KEYBOARD_RELEASED ) )
            {
                modifiers_state ^= modifier;
            }
        }
    }

    return error;
}

void keyboard_driver_consume_event(keyboard_event_t* dst)
{
    while ( keyboard_driver_try_consume_event(dst) == ERRORS_NOT_ENOUGH_DATA )
    {
        __asm__ volatile("hlt");
    }
}

errors_t keyboard_driver_try_consume_unit(keyboard_unit_t* dst)
{
    for (;;)
    {
        keyboard_event_t current_event;
        errors_t error = keyboard_driver_try_consume_event(&current_event);

        if ( error == ERRORS_NOT_ENOUGH_DATA )
        {
            return error;
        }

        if ( !is_terminal_event(&current_event) )
        {
            continue;
        }

        build_unit(dst, &current_event, modifiers_state);

        return error;
    }
}

void keyboard_driver_consume_unit(keyboard_unit_t* dst)
{
    while ( keyboard_driver_try_consume_unit(dst) == ERRORS_NOT_ENOUGH_DATA )
    {
        __asm__ volatile("hlt");
    }
}

void keyboard_driver_set_layout(const keyboard_layout_t* layout)
{
    keyboard_layout = layout;
}



static keyboard_modifiers_type_t mapping_keycode_to_modifier[KEYBOARD_KEYCODE_COUNT] =
{
    [KEYBOARD_KEYCODE_INVALID]                  = KEYBOARD_MODIFIERS_NONE,

    /* first row */
    [KEYBOARD_KEYCODE_ESC]                      = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F1]                       = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F2]                       = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F3]                       = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F4]                       = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F5]                       = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F6]                       = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F7]                       = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F8]                       = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F9]                       = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F10]                      = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F11]                      = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F12]                      = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_PRINT_SCREEN]             = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_SCROLL_LOCK]              = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_PAUSE]                    = KEYBOARD_MODIFIERS_NONE,


    /* second row */
    [KEYBOARD_KEYCODE_BACK_TICK]                = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_1]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_2]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_3]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_4]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_5]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_6]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_7]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_8]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_9]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_0]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_HYPHEN]                   = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_EQUALS]                   = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_BACKSPACE]                = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_INSERT]                   = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_HOME]                     = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_PAGE_UP]                  = KEYBOARD_MODIFIERS_NONE,


    /* third row */
    [KEYBOARD_KEYCODE_TAB]                      = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_Q]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_W]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_E]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_R]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_T]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_Y]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_U]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_I]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_O]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_P]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_BRACKET_L]                = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_BRACKET_R]                = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_BACKSLASH]                = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_DELETE]                   = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_END]                      = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_PAGE_DOWN]                = KEYBOARD_MODIFIERS_NONE,


    /* fourth row */
    [KEYBOARD_KEYCODE_CAPS_LOCK]                = KEYBOARD_MODIFIERS_CAPS_LOCK,
    [KEYBOARD_KEYCODE_A]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_S]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_D]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_F]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_G]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_H]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_J]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_K]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_L]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_SEMICOLON]                = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_SINGLE_QUOTE]             = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_ENTER]                    = KEYBOARD_MODIFIERS_NONE,


    /* fifth row */
    [KEYBOARD_KEYCODE_SHIFT_L]                  = KEYBOARD_MODIFIERS_SHIFT_L,
    [KEYBOARD_KEYCODE_ISO_BACKSLASH]            = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_Z]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_X]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_C]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_V]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_B]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_N]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_M]                        = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_COMMA]                    = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_DOT]                      = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_SLASH]                    = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_SHIFT_R]                  = KEYBOARD_MODIFIERS_SHIFT_R,
    [KEYBOARD_KEYCODE_ARROW_UP]                 = KEYBOARD_MODIFIERS_NONE,


    /* sixth row */
    [KEYBOARD_KEYCODE_APPLE_FN]                 = KEYBOARD_MODIFIERS_APPLE_FN,
    [KEYBOARD_KEYCODE_CONTROL_L]                = KEYBOARD_MODIFIERS_CONTROL_L,
    [KEYBOARD_KEYCODE_SUPER_L]                  = KEYBOARD_MODIFIERS_SUPER_L,
    [KEYBOARD_KEYCODE_ALT_L]                    = KEYBOARD_MODIFIERS_ALT_L,
    [KEYBOARD_KEYCODE_SPACE]                    = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_ALT_R]                    = KEYBOARD_MODIFIERS_ALT_R,
    [KEYBOARD_KEYCODE_SUPER_R]                  = KEYBOARD_MODIFIERS_SUPER_R,
    [KEYBOARD_KEYCODE_PC_MENU]                  = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_CONTROL_R]                = KEYBOARD_MODIFIERS_CONTROL_R,
    [KEYBOARD_KEYCODE_ARROW_LEFT]               = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_ARROW_DOWN]               = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_ARROW_RIGHT]              = KEYBOARD_MODIFIERS_NONE,


    /* KEYPAD */
    [KEYBOARD_KEYCODE_KEYPAD_ASTRIX]            = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_7]                 = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_8]                 = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_9]                 = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_HYPHEN]            = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_4]                 = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_5]                 = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_6]                 = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_PLUS]              = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_1]                 = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_2]                 = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_3]                 = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_0]                 = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_DOT]               = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_ENTER]             = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_SLASH]             = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_KEYPAD_APPLE_EQUALS]      = KEYBOARD_MODIFIERS_NONE,


    /* MISC */
    [KEYBOARD_KEYCODE_MISC_NUM_LOCK]            = KEYBOARD_MODIFIERS_NUM_LOCK,


    /* MULTIMEDIA */
    [KEYBOARD_KEYCODE_MULTIMEDIA_TRACK_PREV]    = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_TRACK_NEXT]    = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_MUTE]          = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_CALCULATOR]    = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_PLAY]          = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_STOP]          = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_VOLUME_DOWN]   = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_VOLUME_UP]     = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_MY_PC]         = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_EMAIL]         = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_MEDIA_SELECT]  = KEYBOARD_MODIFIERS_NONE,

    /* MULTIMEDIA WWW */
    [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_HOME]      = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_SEARCH]    = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_FAVORITES] = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_REFRESH]   = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_STOP]      = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_FORWARD]   = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_BACK]      = KEYBOARD_MODIFIERS_NONE,

    /* ACPI */
    [KEYBOARD_KEYCODE_ACPI_POWER]               = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_ACPI_SLEEP]               = KEYBOARD_MODIFIERS_NONE,
    [KEYBOARD_KEYCODE_ACPI_WAKE]                = KEYBOARD_MODIFIERS_NONE,
};
