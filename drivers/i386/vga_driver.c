#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#include <types.h>
#include <string.h>
#include <vga_driver.h>
#include <cpu_io.h>

#define VGA_DRIVER_BUFFER_ADDRESS 0xB8000
#define VGA_DRIVER_LINE_SIZE (2 * VGA_DRIVER_WIDTH)
#define VGA_DRIVER_SIZE (VGA_DRIVER_LINE_SIZE * VGA_DRIVER_HEIGHT)
#define VGA_DRIVER_SHADOW_SIZE (VGA_DRIVER_LINE_SIZE * VGA_DRIVER_SHADOW_HEIGHT)
#define VGA_DRIVER_DIGITS_ASCII_OFFSET 48
#define VGA_DRIVER_BLANK_LINES 2

#define VGA_DRIVER_PORT_COMMAND 0x3D4
#define VGA_DRIVER_PORT_DATA 0x3D5
#define VGA_DRIVER_COMMAND_SET_CURSOR_HIGH_BYTE 0x0E
#define VGA_DRIVER_COMMAND_SET_CURSOR_LOW_BYTE 0x0F

#define VGA_DRIVER_CONVERT_DIGIT_TO_CHAR(d) (((d) % 10) + VGA_DRIVER_DIGITS_ASCII_OFFSET)
#define VGA_DRIVER_GET_FLAT_INDEX(line, offset) ((line) * VGA_DRIVER_WIDTH + (offset))

#define VGA_DRIVER_SPACE_AND_DEFAULT_COLOR ((word) ((VGA_DRIVER_COLOR_DEFAULT << 8) | ((byte) ' ')))

static inline void carriage_return();
static inline void line_feed();
static inline void shift_shadow_buffer();
static inline void init_shadow_buffer();
static inline void vga_driver_flush_shadow_buffer();

inline static void print_char(byte color, char character);

static size_t shadow_line = 0;
static size_t offset = 0;
static byte shadow_buffer[VGA_DRIVER_SHADOW_SIZE] = { 0 };
static volatile uint8_t* buffer_address = (volatile uint8_t*) VGA_DRIVER_BUFFER_ADDRESS;

static inline void carriage_return()
{
    offset = 0;
}
static inline void line_feed()
{
    ++shadow_line;
    if (shadow_line >= VGA_DRIVER_SHADOW_HEIGHT)
    {
        shift_shadow_buffer();
        shadow_line = VGA_DRIVER_SHADOW_HEIGHT - 1;
    }
}

static void shift_shadow_buffer()
{
    byte* restrict dst;
    byte* restrict src;
    for (size_t i = 0; i < VGA_DRIVER_SHADOW_HEIGHT - 1; ++i)
    {
        dst = shadow_buffer + i * VGA_DRIVER_LINE_SIZE;
        src = shadow_buffer + (i+1) * VGA_DRIVER_LINE_SIZE;
        memcpy(dst, src, VGA_DRIVER_LINE_SIZE);
    }

    word* last_line_word = (word*) src;
    memset_word(last_line_word, VGA_DRIVER_SPACE_AND_DEFAULT_COLOR, VGA_DRIVER_LINE_SIZE);
}

static inline void init_shadow_buffer()
{
    memset_word(shadow_buffer, VGA_DRIVER_SPACE_AND_DEFAULT_COLOR, VGA_DRIVER_SHADOW_SIZE / 2);
}



static inline void vga_driver_flush_shadow_buffer()
{
    size_t writable_lines_count = VGA_DRIVER_HEIGHT - VGA_DRIVER_BLANK_LINES;
    size_t line_to_read_from = (shadow_line >= writable_lines_count)
        ? shadow_line - writable_lines_count + 1
        : 0;

    byte* address_to_read_from = shadow_buffer + line_to_read_from * VGA_DRIVER_LINE_SIZE;
    size_t len = VGA_DRIVER_SIZE;
    memcpy_to_volatile(buffer_address, address_to_read_from, len);
}

static inline void print_char(byte color, char character)
{
    if ( character == '\n' )
    {
        line_feed();
        carriage_return();

        vga_driver_flush_shadow_buffer();

        return;
    }

    if (offset >= VGA_DRIVER_WIDTH)
    {
        line_feed();
        carriage_return();
    }

    size_t index = 2 * VGA_DRIVER_GET_FLAT_INDEX(shadow_line, offset);
    shadow_buffer[index] = (byte)character;
    shadow_buffer[index + 1] = color;
    ++offset;
}

void vga_driver_print_string_colored(byte color, const char* string)
{
    while (*string != '\0')
    {
        print_char(color, *string);
        ++string;
    }
}

void vga_driver_init(const vga_driver_settings_t* settings)
{
    init_shadow_buffer();

    shadow_line = settings->initial_line;
    if (settings->should_copy_vga_buffer)
    {
        memcpy_from_volatile(shadow_buffer, buffer_address, VGA_DRIVER_SIZE);
    }
}
