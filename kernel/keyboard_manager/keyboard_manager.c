#include "include/keyboard_manager.h"
#include <keyboard_driver.h>
#include <keyboard_types.h>

static keyboard_layout_t map_layout_type_to_layout[KEYBOARD_LAYOUT_COUNT];

void keyboard_manager_set_layout(keyboard_layout_type_t layout_type)
{
    keyboard_driver_set_layout( &map_layout_type_to_layout[layout_type] );
}

static keyboard_layout_t map_layout_type_to_layout[KEYBOARD_LAYOUT_COUNT] =
{
    [KEYBOARD_LAYOUT_US_QWERTY] = (keyboard_layout_t)
    {
        .map_normal =
        {
            [KEYBOARD_KEYCODE_INVALID]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,

            /* first row */
            [KEYBOARD_KEYCODE_ESC]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F1]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F2]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F3]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F4]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F5]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F6]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F7]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F8]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F9]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F10]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F11]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F12]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PRINT_SCREEN]             = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SCROLL_LOCK]              = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PAUSE]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* second row */
            [KEYBOARD_KEYCODE_BACK_TICK]                = '`',
            [KEYBOARD_KEYCODE_1]                        = '1',
            [KEYBOARD_KEYCODE_2]                        = '2',
            [KEYBOARD_KEYCODE_3]                        = '3',
            [KEYBOARD_KEYCODE_4]                        = '4',
            [KEYBOARD_KEYCODE_5]                        = '5',
            [KEYBOARD_KEYCODE_6]                        = '6',
            [KEYBOARD_KEYCODE_7]                        = '7',
            [KEYBOARD_KEYCODE_8]                        = '8',
            [KEYBOARD_KEYCODE_9]                        = '9',
            [KEYBOARD_KEYCODE_0]                        = '0',
            [KEYBOARD_KEYCODE_HYPHEN]                   = '-',
            [KEYBOARD_KEYCODE_EQUALS]                   = '=',
            [KEYBOARD_KEYCODE_BACKSPACE]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_INSERT]                   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_HOME]                     = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PAGE_UP]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* third row */
            [KEYBOARD_KEYCODE_TAB]                      = '\t',
            [KEYBOARD_KEYCODE_Q]                        = 'q',
            [KEYBOARD_KEYCODE_W]                        = 'w',
            [KEYBOARD_KEYCODE_E]                        = 'e',
            [KEYBOARD_KEYCODE_R]                        = 'r',
            [KEYBOARD_KEYCODE_T]                        = 't',
            [KEYBOARD_KEYCODE_Y]                        = 'y',
            [KEYBOARD_KEYCODE_U]                        = 'u',
            [KEYBOARD_KEYCODE_I]                        = 'i',
            [KEYBOARD_KEYCODE_O]                        = 'o',
            [KEYBOARD_KEYCODE_P]                        = 'p',
            [KEYBOARD_KEYCODE_BRACKET_L]                = '[',
            [KEYBOARD_KEYCODE_BRACKET_R]                = ']',
            [KEYBOARD_KEYCODE_BACKSLASH]                = '\\',
            [KEYBOARD_KEYCODE_DELETE]                   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_END]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PAGE_DOWN]                = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* fourth row */
            [KEYBOARD_KEYCODE_CAPS_LOCK]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_A]                        = 'a',
            [KEYBOARD_KEYCODE_S]                        = 's',
            [KEYBOARD_KEYCODE_D]                        = 'd',
            [KEYBOARD_KEYCODE_F]                        = 'f',
            [KEYBOARD_KEYCODE_G]                        = 'g',
            [KEYBOARD_KEYCODE_H]                        = 'h',
            [KEYBOARD_KEYCODE_J]                        = 'j',
            [KEYBOARD_KEYCODE_K]                        = 'k',
            [KEYBOARD_KEYCODE_L]                        = 'l',
            [KEYBOARD_KEYCODE_SEMICOLON]                = ';',
            [KEYBOARD_KEYCODE_SINGLE_QUOTE]             = '\'',
            [KEYBOARD_KEYCODE_ENTER]                    = '\n',


            /* fifth row */
            [KEYBOARD_KEYCODE_SHIFT_L]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ISO_BACKSLASH]            = '\\',
            [KEYBOARD_KEYCODE_Z]                        = 'z',
            [KEYBOARD_KEYCODE_X]                        = 'x',
            [KEYBOARD_KEYCODE_C]                        = 'c',
            [KEYBOARD_KEYCODE_V]                        = 'v',
            [KEYBOARD_KEYCODE_B]                        = 'b',
            [KEYBOARD_KEYCODE_N]                        = 'n',
            [KEYBOARD_KEYCODE_M]                        = 'm',
            [KEYBOARD_KEYCODE_COMMA]                    = ',',
            [KEYBOARD_KEYCODE_DOT]                      = '.',
            [KEYBOARD_KEYCODE_SLASH]                    = '/',
            [KEYBOARD_KEYCODE_SHIFT_R]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_UP]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* sixth row */
            [KEYBOARD_KEYCODE_APPLE_FN]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_CONTROL_L]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SUPER_L]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ALT_L]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SPACE]                    = ' ',
            [KEYBOARD_KEYCODE_ALT_R]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SUPER_R]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PC_MENU]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_CONTROL_R]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_LEFT]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_DOWN]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_RIGHT]              = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* KEYPAD */
            [KEYBOARD_KEYCODE_KEYPAD_ASTRIX]            = '*',
            [KEYBOARD_KEYCODE_KEYPAD_7]                 = '7',
            [KEYBOARD_KEYCODE_KEYPAD_8]                 = '8',
            [KEYBOARD_KEYCODE_KEYPAD_9]                 = '9',
            [KEYBOARD_KEYCODE_KEYPAD_HYPHEN]            = '-',
            [KEYBOARD_KEYCODE_KEYPAD_4]                 = '4',
            [KEYBOARD_KEYCODE_KEYPAD_5]                 = '5',
            [KEYBOARD_KEYCODE_KEYPAD_6]                 = '6',
            [KEYBOARD_KEYCODE_KEYPAD_PLUS]              = '+',
            [KEYBOARD_KEYCODE_KEYPAD_1]                 = '1',
            [KEYBOARD_KEYCODE_KEYPAD_2]                 = '2',
            [KEYBOARD_KEYCODE_KEYPAD_3]                 = '3',
            [KEYBOARD_KEYCODE_KEYPAD_0]                 = '0',
            [KEYBOARD_KEYCODE_KEYPAD_DOT]               = '.',
            [KEYBOARD_KEYCODE_KEYPAD_ENTER]             = '\n',
            [KEYBOARD_KEYCODE_KEYPAD_SLASH]             = '/',
            [KEYBOARD_KEYCODE_KEYPAD_APPLE_EQUALS]      = '=',


            /* MISC */
            [KEYBOARD_KEYCODE_MISC_NUM_LOCK]            = KEYBOARD_NON_RESOLVABLE_UNICODE,



            /* MULTIMEDIA */
            [KEYBOARD_KEYCODE_MULTIMEDIA_TRACK_PREV]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_TRACK_NEXT]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_MUTE]          = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_CALCULATOR]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_PLAY]          = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_STOP]          = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_VOLUME_DOWN]   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_VOLUME_UP]     = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_MY_PC]         = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_EMAIL]         = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_MEDIA_SELECT]  = KEYBOARD_NON_RESOLVABLE_UNICODE,

            /* MULTIMEDIA WWW */
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_HOME]      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_SEARCH]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_FAVORITES] = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_REFRESH]   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_STOP]      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_FORWARD]   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_BACK]      = KEYBOARD_NON_RESOLVABLE_UNICODE,

            /* ACPI */
            [KEYBOARD_KEYCODE_ACPI_POWER]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ACPI_SLEEP]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ACPI_WAKE]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
        },
        .map_shifted =
        {
            [KEYBOARD_KEYCODE_INVALID]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,

            /* first row */
            [KEYBOARD_KEYCODE_ESC]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F1]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F2]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F3]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F4]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F5]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F6]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F7]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F8]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F9]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F10]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F11]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F12]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PRINT_SCREEN]             = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SCROLL_LOCK]              = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PAUSE]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* second row */
            [KEYBOARD_KEYCODE_BACK_TICK]                = '~',
            [KEYBOARD_KEYCODE_1]                        = '!',
            [KEYBOARD_KEYCODE_2]                        = '@',
            [KEYBOARD_KEYCODE_3]                        = '#',
            [KEYBOARD_KEYCODE_4]                        = '$',
            [KEYBOARD_KEYCODE_5]                        = '%',
            [KEYBOARD_KEYCODE_6]                        = '^',
            [KEYBOARD_KEYCODE_7]                        = '&',
            [KEYBOARD_KEYCODE_8]                        = '*',
            [KEYBOARD_KEYCODE_9]                        = '(',
            [KEYBOARD_KEYCODE_0]                        = ')',
            [KEYBOARD_KEYCODE_HYPHEN]                   = '_',
            [KEYBOARD_KEYCODE_EQUALS]                   = '+',
            [KEYBOARD_KEYCODE_BACKSPACE]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_INSERT]                   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_HOME]                     = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PAGE_UP]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* third row */
            [KEYBOARD_KEYCODE_TAB]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_Q]                        = 'Q',
            [KEYBOARD_KEYCODE_W]                        = 'W',
            [KEYBOARD_KEYCODE_E]                        = 'E',
            [KEYBOARD_KEYCODE_R]                        = 'R',
            [KEYBOARD_KEYCODE_T]                        = 'T',
            [KEYBOARD_KEYCODE_Y]                        = 'Y',
            [KEYBOARD_KEYCODE_U]                        = 'U',
            [KEYBOARD_KEYCODE_I]                        = 'I',
            [KEYBOARD_KEYCODE_O]                        = 'O',
            [KEYBOARD_KEYCODE_P]                        = 'P',
            [KEYBOARD_KEYCODE_BRACKET_L]                = '{',
            [KEYBOARD_KEYCODE_BRACKET_R]                = '}',
            [KEYBOARD_KEYCODE_BACKSLASH]                = '|',
            [KEYBOARD_KEYCODE_DELETE]                   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_END]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PAGE_DOWN]                = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* fourth row */
            [KEYBOARD_KEYCODE_CAPS_LOCK]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_A]                        = 'A',
            [KEYBOARD_KEYCODE_S]                        = 'S',
            [KEYBOARD_KEYCODE_D]                        = 'D',
            [KEYBOARD_KEYCODE_F]                        = 'F',
            [KEYBOARD_KEYCODE_G]                        = 'G',
            [KEYBOARD_KEYCODE_H]                        = 'H',
            [KEYBOARD_KEYCODE_J]                        = 'J',
            [KEYBOARD_KEYCODE_K]                        = 'K',
            [KEYBOARD_KEYCODE_L]                        = 'L',
            [KEYBOARD_KEYCODE_SEMICOLON]                = ':',
            [KEYBOARD_KEYCODE_SINGLE_QUOTE]             = '"',
            [KEYBOARD_KEYCODE_ENTER]                    = '\n',


            /* fifth row */
            [KEYBOARD_KEYCODE_SHIFT_L]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ISO_BACKSLASH]            = '|',
            [KEYBOARD_KEYCODE_Z]                        = 'Z',
            [KEYBOARD_KEYCODE_X]                        = 'X',
            [KEYBOARD_KEYCODE_C]                        = 'C',
            [KEYBOARD_KEYCODE_V]                        = 'V',
            [KEYBOARD_KEYCODE_B]                        = 'B',
            [KEYBOARD_KEYCODE_N]                        = 'N',
            [KEYBOARD_KEYCODE_M]                        = 'M',
            [KEYBOARD_KEYCODE_COMMA]                    = '<',
            [KEYBOARD_KEYCODE_DOT]                      = '>',
            [KEYBOARD_KEYCODE_SLASH]                    = '?',
            [KEYBOARD_KEYCODE_SHIFT_R]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_UP]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* sixth row */
            [KEYBOARD_KEYCODE_APPLE_FN]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_CONTROL_L]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SUPER_L]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ALT_L]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SPACE]                    = ' ',
            [KEYBOARD_KEYCODE_ALT_R]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SUPER_R]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PC_MENU]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_CONTROL_R]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_LEFT]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_DOWN]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_RIGHT]              = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* KEYPAD */
            [KEYBOARD_KEYCODE_KEYPAD_ASTRIX]            = '*',
            [KEYBOARD_KEYCODE_KEYPAD_7]                 = '7',
            [KEYBOARD_KEYCODE_KEYPAD_8]                 = '8',
            [KEYBOARD_KEYCODE_KEYPAD_9]                 = '9',
            [KEYBOARD_KEYCODE_KEYPAD_HYPHEN]            = '-',
            [KEYBOARD_KEYCODE_KEYPAD_4]                 = '4',
            [KEYBOARD_KEYCODE_KEYPAD_5]                 = '5',
            [KEYBOARD_KEYCODE_KEYPAD_6]                 = '6',
            [KEYBOARD_KEYCODE_KEYPAD_PLUS]              = '+',
            [KEYBOARD_KEYCODE_KEYPAD_1]                 = '1',
            [KEYBOARD_KEYCODE_KEYPAD_2]                 = '2',
            [KEYBOARD_KEYCODE_KEYPAD_3]                 = '3',
            [KEYBOARD_KEYCODE_KEYPAD_0]                 = '0',
            [KEYBOARD_KEYCODE_KEYPAD_DOT]               = '.',
            [KEYBOARD_KEYCODE_KEYPAD_ENTER]             = '\n',
            [KEYBOARD_KEYCODE_KEYPAD_SLASH]             = '/',
            [KEYBOARD_KEYCODE_KEYPAD_APPLE_EQUALS]      = '=',


            /* MISC */
            [KEYBOARD_KEYCODE_MISC_NUM_LOCK]            = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* MULTIMEDIA */
            [KEYBOARD_KEYCODE_MULTIMEDIA_TRACK_PREV]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_TRACK_NEXT]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_MUTE]          = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_CALCULATOR]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_PLAY]          = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_STOP]          = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_VOLUME_DOWN]   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_VOLUME_UP]     = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_MY_PC]         = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_EMAIL]         = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_MEDIA_SELECT]  = KEYBOARD_NON_RESOLVABLE_UNICODE,

            /* MULTIMEDIA WWW */
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_HOME]      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_SEARCH]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_FAVORITES] = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_REFRESH]   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_STOP]      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_FORWARD]   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_BACK]      = KEYBOARD_NON_RESOLVABLE_UNICODE,

            /* ACPI */
            [KEYBOARD_KEYCODE_ACPI_POWER]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ACPI_SLEEP]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ACPI_WAKE]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
        },
        .map_alted =
        {
            [KEYBOARD_KEYCODE_INVALID]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,

            /* first row */
            [KEYBOARD_KEYCODE_ESC]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F1]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F2]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F3]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F4]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F5]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F6]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F7]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F8]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F9]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F10]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F11]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F12]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PRINT_SCREEN]             = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SCROLL_LOCK]              = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PAUSE]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* second row */
            [KEYBOARD_KEYCODE_BACK_TICK]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_1]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_2]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_3]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_4]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_5]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_6]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_7]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_8]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_9]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_0]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_HYPHEN]                   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_EQUALS]                   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_BACKSPACE]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_INSERT]                   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_HOME]                     = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PAGE_UP]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* third row */
            [KEYBOARD_KEYCODE_TAB]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_Q]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_W]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_E]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_R]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_T]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_Y]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_U]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_I]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_O]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_P]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_BRACKET_L]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_BRACKET_R]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_BACKSLASH]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_DELETE]                   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_END]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PAGE_DOWN]                = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* fourth row */
            [KEYBOARD_KEYCODE_CAPS_LOCK]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_A]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_S]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_D]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_G]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_H]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_J]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_K]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_L]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SEMICOLON]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SINGLE_QUOTE]             = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ENTER]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* fifth row */
            [KEYBOARD_KEYCODE_SHIFT_L]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ISO_BACKSLASH]            = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_Z]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_X]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_C]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_V]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_B]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_N]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_M]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_COMMA]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_DOT]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SLASH]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SHIFT_R]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_UP]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* sixth row */
            [KEYBOARD_KEYCODE_APPLE_FN]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_CONTROL_L]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SUPER_L]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ALT_L]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SPACE]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ALT_R]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SUPER_R]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PC_MENU]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_CONTROL_R]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_LEFT]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_DOWN]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_RIGHT]              = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* KEYPAD */
            [KEYBOARD_KEYCODE_KEYPAD_ASTRIX]            = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_7]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_8]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_9]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_HYPHEN]            = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_4]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_5]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_6]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_PLUS]              = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_1]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_2]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_3]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_0]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_DOT]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_ENTER]             = '\n',
            [KEYBOARD_KEYCODE_KEYPAD_SLASH]             = '/',
            [KEYBOARD_KEYCODE_KEYPAD_APPLE_EQUALS]      = '=',


            /* MISC */
            [KEYBOARD_KEYCODE_MISC_NUM_LOCK]            = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* MULTIMEDIA */
            [KEYBOARD_KEYCODE_MULTIMEDIA_TRACK_PREV]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_TRACK_NEXT]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_MUTE]          = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_CALCULATOR]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_PLAY]          = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_STOP]          = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_VOLUME_DOWN]   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_VOLUME_UP]     = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_MY_PC]         = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_EMAIL]         = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_MEDIA_SELECT]  = KEYBOARD_NON_RESOLVABLE_UNICODE,

            /* MULTIMEDIA WWW */
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_HOME]      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_SEARCH]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_FAVORITES] = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_REFRESH]   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_STOP]      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_FORWARD]   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_BACK]      = KEYBOARD_NON_RESOLVABLE_UNICODE,

            /* ACPI */
            [KEYBOARD_KEYCODE_ACPI_POWER]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ACPI_SLEEP]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ACPI_WAKE]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
        },
        .map_shifted_alted =
        {
            [KEYBOARD_KEYCODE_INVALID]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,

            /* first row */
            [KEYBOARD_KEYCODE_ESC]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F1]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F2]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F3]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F4]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F5]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F6]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F7]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F8]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F9]                       = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F10]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F11]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F12]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PRINT_SCREEN]             = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SCROLL_LOCK]              = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PAUSE]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* second row */
            [KEYBOARD_KEYCODE_BACK_TICK]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_1]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_2]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_3]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_4]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_5]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_6]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_7]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_8]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_9]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_0]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_HYPHEN]                   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_EQUALS]                   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_BACKSPACE]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_INSERT]                   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_HOME]                     = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PAGE_UP]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* third row */
            [KEYBOARD_KEYCODE_TAB]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_Q]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_W]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_E]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_R]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_T]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_Y]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_U]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_I]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_O]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_P]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_BRACKET_L]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_BRACKET_R]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_BACKSLASH]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_DELETE]                   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_END]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PAGE_DOWN]                = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* fourth row */
            [KEYBOARD_KEYCODE_CAPS_LOCK]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_A]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_S]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_D]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_F]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_G]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_H]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_J]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_K]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_L]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SEMICOLON]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SINGLE_QUOTE]             = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ENTER]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* fifth row */
            [KEYBOARD_KEYCODE_SHIFT_L]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ISO_BACKSLASH]            = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_Z]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_X]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_C]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_V]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_B]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_N]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_M]                        = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_COMMA]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_DOT]                      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SLASH]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SHIFT_R]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_UP]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* sixth row */
            [KEYBOARD_KEYCODE_APPLE_FN]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_CONTROL_L]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SUPER_L]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ALT_L]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SPACE]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ALT_R]                    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_SUPER_R]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_PC_MENU]                  = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_CONTROL_R]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_LEFT]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_DOWN]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ARROW_RIGHT]              = KEYBOARD_NON_RESOLVABLE_UNICODE,


            /* KEYPAD */
            [KEYBOARD_KEYCODE_KEYPAD_ASTRIX]            = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_7]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_8]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_9]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_HYPHEN]            = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_4]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_5]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_6]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_PLUS]              = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_1]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_2]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_3]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_0]                 = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_DOT]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_KEYPAD_ENTER]             = '\n',
            [KEYBOARD_KEYCODE_KEYPAD_SLASH]             = '/',
            [KEYBOARD_KEYCODE_KEYPAD_APPLE_EQUALS]      = '=',


            /* MISC */
            [KEYBOARD_KEYCODE_MISC_NUM_LOCK]            = KEYBOARD_NON_RESOLVABLE_UNICODE,



            /* MULTIMEDIA */
            [KEYBOARD_KEYCODE_MULTIMEDIA_TRACK_PREV]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_TRACK_NEXT]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_MUTE]          = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_CALCULATOR]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_PLAY]          = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_STOP]          = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_VOLUME_DOWN]   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_VOLUME_UP]     = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_MY_PC]         = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_EMAIL]         = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_MEDIA_SELECT]  = KEYBOARD_NON_RESOLVABLE_UNICODE,

            /* MULTIMEDIA WWW */
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_HOME]      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_SEARCH]    = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_FAVORITES] = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_REFRESH]   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_STOP]      = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_FORWARD]   = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_MULTIMEDIA_WWW_BACK]      = KEYBOARD_NON_RESOLVABLE_UNICODE,

            /* ACPI */
            [KEYBOARD_KEYCODE_ACPI_POWER]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ACPI_SLEEP]               = KEYBOARD_NON_RESOLVABLE_UNICODE,
            [KEYBOARD_KEYCODE_ACPI_WAKE]                = KEYBOARD_NON_RESOLVABLE_UNICODE,
        }
    }
};
