#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#include <types.h>
#include <string.h>
#include <vga_driver.h>
#include <io.h>

#define VGA_DRIVER_BUFFER_ADDRESS 0xB8000
#define VGA_DRIVER_WIDTH 80
#define VGA_DRIVER_HEIGHT 25
#define VGA_DRIVER_SHADOW_HEIGHT 40
#define VGA_DRIVER_LINE_SIZE (2 * VGA_DRIVER_WIDTH)
#define VGA_DRIVER_SIZE (VGA_DRIVER_LINE_SIZE * VGA_DRIVER_HEIGHT)
#define VGA_DRIVER_SHADOW_SIZE (VGA_DRIVER_LINE_SIZE * VGA_DRIVER_SHADOW_HEIGHT)
#define VGA_DRIVER_GREY_ON_BLACK 0x07
#define VGA_DRIVER_DIGITS_ASCII_OFFSET 48
#define VGA_DRIVER_BLANK_LINES 2

#define VGA_DRIVER_PORT_COMMAND 0x3D4
#define VGA_DRIVER_PORT_DATA 0x3D5
#define VGA_DRIVER_COMMAND_SET_CURSOR_HIGH_BYTE 0x0E
#define VGA_DRIVER_COMMAND_SET_CURSOR_LOW_BYTE 0x0F

#define VGA_DRIVER_CONVERT_DIGIT_TO_CHAR(d) (((d) % 10) + VGA_DRIVER_DIGITS_ASCII_OFFSET)
#define VGA_DRIVER_GET_FLAT_INDEX(line, offset) ((line) * VGA_DRIVER_WIDTH + (offset))
#define VGA_DRIVER_PRINT_STRING(string) print_string_colored((string), VGA_DRIVER_GREY_ON_BLACK);

static inline void carriage_return();
static inline void line_feed();
static inline void shift_shadow_buffer();

static void flush_shadow_buffer();
static void move_cursor(size_t shadow_line, size_t offset);
static void print_char(char character, byte color);
static void print_int(intmax_t d);
static void print_string_colored(const char* string, byte color);
static void printf_colored(byte color, const char* format, ...);
static void vprintf_colored(byte color, const char* format, va_list ap);

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

    memset(src, 0, VGA_DRIVER_LINE_SIZE); // Sets the backgroup black, in the future I might want to write a word-memcpy, and use it to print ' ' in the default color. The current default is black, and \0 looks just like ' ' so I'm ignoring it
}

static inline void flush_shadow_buffer()
{
    size_t writable_lines_count = VGA_DRIVER_HEIGHT - VGA_DRIVER_BLANK_LINES;
    size_t line_to_read_from = (shadow_line > writable_lines_count)
        ? shadow_line - writable_lines_count
        : 0;

    byte* address_to_read_from = shadow_buffer + line_to_read_from * VGA_DRIVER_LINE_SIZE;
    size_t len = VGA_DRIVER_SIZE - VGA_DRIVER_BLANK_LINES * VGA_DRIVER_LINE_SIZE;
    memcpy(buffer_address, address_to_read_from, len);
}

static void move_cursor(size_t shadow_line, size_t offset)
{
    size_t cursor_line = shadow_line;
    if (shadow_line > VGA_DRIVER_HEIGHT - VGA_DRIVER_BLANK_LINES)
    {
        cursor_line = VGA_DRIVER_HEIGHT - VGA_DRIVER_BLANK_LINES;
    }

    word index = (word) VGA_DRIVER_GET_FLAT_INDEX(cursor_line, offset);
    byte index_low = (byte) (index & 0xFF);
    byte index_high = (byte) (index >> 8);

    outb(VGA_DRIVER_PORT_COMMAND, VGA_DRIVER_COMMAND_SET_CURSOR_HIGH_BYTE);
    outb(VGA_DRIVER_PORT_DATA, index_high);

    outb(VGA_DRIVER_PORT_COMMAND, VGA_DRIVER_COMMAND_SET_CURSOR_LOW_BYTE);
    outb(VGA_DRIVER_PORT_DATA, index_low);
}

static void print_char(char character, byte color)
{
    if (character == '\n')
    {
        line_feed();
        carriage_return();

        flush_shadow_buffer();
        move_cursor(shadow_line, offset);

        if (shadow_line >= VGA_DRIVER_SHADOW_HEIGHT)
        {
            shift_shadow_buffer();
            shadow_line = VGA_DRIVER_SHADOW_HEIGHT - 1;
        }

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

static void print_int(intmax_t d)
{
    bool is_negative = d < 0;
    char digits[21]; // int max is 10 digits and null terminator is another
    digits[20] = '\0';

    size_t current_idx = 19;
    do
    {
        int8_t current_digit = d % 10;
        if (current_digit < 0)
        {
            current_digit *= -1;
        }
        digits[current_idx] = VGA_DRIVER_CONVERT_DIGIT_TO_CHAR(current_digit);
        --current_idx;
        d /= 10;
    } while (d != 0);

    if (is_negative)
    {
        digits[current_idx] = '-';
        --current_idx;
    }

    ++current_idx; // we decremented it one too many times
    char* final_string = ((char*)digits) + current_idx;
    VGA_DRIVER_PRINT_STRING(final_string);
}

static void print_string_colored(const char* string, byte color)
{
    while (*string != '\0')
    {
        print_char(*string, color);
        ++string;
    }
}

static inline void print_string(const char* string)
{
    print_string_colored(string, VGA_DRIVER_GREY_ON_BLACK);
}

static void vprintf_colored(byte color, const char* format, va_list ap)
{
    char* s;
    char c;
    int d;
    long d_long;
    long long d_long_long;

    while (*format)
    {
        char current = *format;
        uint8_t long_count = 0;

        if (current == '%')
        {
            ++format;
            current = *format;

            if (current == 'l')
            {
                ++long_count;
                ++format;
                current = *format;
                if (current == 'l')
                {
                    ++format;
                    current = *format;
                    ++long_count;
                }
            }

            switch (current)
            {
                case 's':
                    s = va_arg(ap, char*);
                    VGA_DRIVER_PRINT_STRING(s);
                    break;
                case 'c':
                    c = (char)va_arg(ap, int);
                    print_char(c, VGA_DRIVER_GREY_ON_BLACK);
                    break;
                case 'd':
                    if (long_count == 0)
                    {
                        d = va_arg(ap, int);
                        print_int(d);
                        break;
                    }
                    else if (long_count == 1)
                    {
                        d_long = va_arg(ap, long);
                        print_int(d_long);
                        break;
                    }
                    else if (long_count == 2)
                    {
                        d_long_long = va_arg(ap, long long);
                        print_int(d_long_long);
                        break;
                    }

            }
        }
        else
        {
            print_char(current, VGA_DRIVER_GREY_ON_BLACK);
        }
        ++format;
    }
}

static void printf_colored(byte color, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);

    vprintf_colored(color, format, ap);

    va_end(ap);
}

void vga_driver_report(const char* message, vga_driver_report_status status)
{
    VGA_DRIVER_PRINT_STRING("[");
    if (status == VGA_DRIVER_SUCCESS)
    {
        print_string_colored(" SUCCESS ", 0x02);
    }
    else if (status == VGA_DRIVER_FAILURE)
    {
        print_string_colored(" FAILURE ", 0x04);
    }

    vga_driver_printf("] %s\n", message);
}

void vga_driver_printf(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);

    vprintf_colored(VGA_DRIVER_GREY_ON_BLACK, format, ap);

    va_end(ap);
}
void vga_driver_clear()
{
    word space_and_default_color = (VGA_DRIVER_GREY_ON_BLACK << 8) | ((byte) ' ');
    memset_word(shadow_buffer, space_and_default_color, VGA_DRIVER_SHADOW_SIZE);
    shadow_line = 0;
    offset = 0;
    flush_shadow_buffer();
    move_cursor(shadow_line, offset);
}

void vga_driver_init(const vga_driver_settings_t* settings)
{
    shadow_line = settings->initial_line;
    if (settings->should_copy_vga_buffer)
    {
        memcpy(shadow_buffer, buffer_address, VGA_DRIVER_SIZE);
    }
}
