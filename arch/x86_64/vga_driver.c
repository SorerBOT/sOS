#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#include <types.h>
#include <string.h>
#include <vga_driver.h>
#include <cpu_io.h>
#include <vmm.h>

#define VGA_DRIVER_BUFFER_ADDRESS (VMM_TRANSLATE_PHYSICAL_TO_KERNEL_MAP(0xB8000))
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

static inline void carriage_return(void);
static inline void line_feed(void);
static inline void back_space(void);
static inline void shift_shadow_buffer(void);
static inline void init_shadow_buffer(void);

static void move_cursor(size_t shadow_line, size_t offset);
static void print_char(byte color, char character);

static size_t shadow_line = 0;
static size_t offset = 0;
static byte shadow_buffer[VGA_DRIVER_SHADOW_SIZE] = { 0 };
static volatile uint8_t* buffer_address = (volatile uint8_t*) VGA_DRIVER_BUFFER_ADDRESS;

static inline void carriage_return(void)
{
    offset = 0;
}

static inline void line_feed(void)
{
    ++shadow_line;
    if (shadow_line >= VGA_DRIVER_SHADOW_HEIGHT)
    {
        shift_shadow_buffer();
        shadow_line = VGA_DRIVER_SHADOW_HEIGHT - 1;
    }
}

static inline void back_space(void)
{
    if ( offset > 0 )
    {
        --offset;
    }
    else if ( shadow_line > 0 )
    {
        --shadow_line;
        offset = VGA_DRIVER_WIDTH - 1;
    }
}

static void shift_shadow_buffer(void)
{
    byte* dst;
    byte* src;
    for (size_t i = 1; i < VGA_DRIVER_SHADOW_HEIGHT; ++i)
    {
        dst = shadow_buffer + (i-1) * VGA_DRIVER_LINE_SIZE;
        src = shadow_buffer + (i) * VGA_DRIVER_LINE_SIZE;
        memcpy(dst, src, VGA_DRIVER_LINE_SIZE);
    }

    word* last_line_word = (word*) src;
    memset_word(last_line_word, VGA_DRIVER_SPACE_AND_DEFAULT_COLOR, VGA_DRIVER_LINE_SIZE / 2);
}

static inline void init_shadow_buffer(void)
{
    memset_word(shadow_buffer, VGA_DRIVER_SPACE_AND_DEFAULT_COLOR, VGA_DRIVER_SHADOW_SIZE / 2);
}


void vga_driver_flush_shadow_buffer(void)
{
    size_t writable_lines_count = VGA_DRIVER_HEIGHT - VGA_DRIVER_BLANK_LINES;
    size_t line_to_read_from = (shadow_line >= writable_lines_count)
        ? shadow_line - writable_lines_count + 1
        : 0;

    byte* address_to_read_from = ((byte*) shadow_buffer) + line_to_read_from * VGA_DRIVER_LINE_SIZE;
    size_t len = writable_lines_count * VGA_DRIVER_LINE_SIZE;
    memcpy_to_volatile(buffer_address, address_to_read_from, len);
    move_cursor(shadow_line, offset);
}

static void move_cursor(size_t shadow_line, size_t offset)
{
    size_t cursor_line = shadow_line;
    if (shadow_line >= VGA_DRIVER_HEIGHT - VGA_DRIVER_BLANK_LINES)
    {
        cursor_line = VGA_DRIVER_HEIGHT - VGA_DRIVER_BLANK_LINES - 1;
    }

    word index = (word) VGA_DRIVER_GET_FLAT_INDEX(cursor_line, offset);
    byte index_low = (byte) (index & 0xFF);
    byte index_high = (byte) (index >> 8);

    cpu_io_write_byte(VGA_DRIVER_PORT_COMMAND, VGA_DRIVER_COMMAND_SET_CURSOR_HIGH_BYTE);
    cpu_io_write_byte(VGA_DRIVER_PORT_DATA, index_high);

    cpu_io_write_byte(VGA_DRIVER_PORT_COMMAND, VGA_DRIVER_COMMAND_SET_CURSOR_LOW_BYTE);
    cpu_io_write_byte(VGA_DRIVER_PORT_DATA, index_low);
}

static void print_char(byte color, char character)
{
    if ( character == '\b' )
    {
        back_space();
        return;
    }

    if (character == '\n')
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

void vga_driver_clear_colored(byte new_background_color)
{
    word space_and_color = (new_background_color << 8) | ((byte) ' ');
    memset_word(shadow_buffer, space_and_color, VGA_DRIVER_SHADOW_SIZE / 2);
    shadow_line = 0;
    offset = 0;
    vga_driver_flush_shadow_buffer();
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
