#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#include <types.h>
#include <string.h>
#include <vga_driver.h>

#define VGA_DRIVER_BUFFER_ADDRESS 0xB8000
#define VGA_DRIVER_WIDTH 80
#define VGA_DRIVER_HEIGHT 25
#define VGA_DRIVER_SCROLL_HEIGHT 40
#define VGA_DRIVER_LINE_SIZE (2 * VGA_DRIVER_WIDTH)
#define VGA_DRIVER_SIZE (VGA_DRIVER_LINE_SIZE * VGA_DRIVER_HEIGHT)
#define VGA_DRIVER_MAGENTA_ON_BLACK 0x07
#define VGA_DRIVER_DIGITS_ASCII_OFFSET 48
#define VGA_DRIVER_CONVERT_DIGIT_TO_CHAR(d) (((d) % 10) + VGA_DRIVER_DIGITS_ASCII_OFFSET)

static inline void carriage_return();
static inline void line_feed();
static inline void flush_scroll_buffer();
static void shift_scroll_buffer();
static void print_char(char character, uint8_t color);
static void print_int(intmax_t d);
static void print_string(const char* string, uint8_t color);

static size_t line = 0;
static size_t offset = 0;
static byte scroll_buffer[VGA_DRIVER_SCROLL_HEIGHT * VGA_DRIVER_LINE_SIZE];
volatile uint8_t* buffer_address = (volatile uint8_t*) VGA_DRIVER_BUFFER_ADDRESS;

static inline void carriage_return()
{
    offset = 0;
}
static inline void line_feed()
{
    ++line;
}

static void shift_scroll_buffer()
{
    byte* restrict dst;
    byte* restrict src;
    for (size_t i = 0; i < VGA_DRIVER_SCROLL_HEIGHT - 1; ++i)
    {
        dst = scroll_buffer + i * VGA_DRIVER_LINE_SIZE;
        src = scroll_buffer + (i+1) * VGA_DRIVER_LINE_SIZE;
        memcpy(dst, src, VGA_DRIVER_LINE_SIZE);
    }

    memset(src, 0, VGA_DRIVER_LINE_SIZE); // Sets the backgroup black, in the future I might want to write a word-memcpy, and use it to print ' ' in the default color. The current default is black, and \0 looks just like ' ' so I'm ignoring it
}

static inline void flush_scroll_buffer()
{
    size_t line_to_read_from = (line > VGA_DRIVER_HEIGHT)
        ? line - VGA_DRIVER_HEIGHT
        : 0;
    memcpy(buffer_address, scroll_buffer + line_to_read_from * VGA_DRIVER_LINE_SIZE, VGA_DRIVER_SIZE);
}

static void print_char(char character, uint8_t color)
{
    if (character == '\n')
    {
        line_feed();
        carriage_return();
        flush_scroll_buffer();
        return;
    }

    if (offset >= VGA_DRIVER_WIDTH)
    {
        line_feed();
        carriage_return();
    }

    if (line >= VGA_DRIVER_SCROLL_HEIGHT)
    {
        shift_scroll_buffer();
        line = VGA_DRIVER_SCROLL_HEIGHT - 1;
    }

    size_t index = (line * VGA_DRIVER_WIDTH + offset) * 2;
    scroll_buffer[index] = (byte)character;
    scroll_buffer[index + 1] = color;
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
    print_string(final_string, VGA_DRIVER_MAGENTA_ON_BLACK);
}

static void print_string(const char* string, uint8_t color)
{
    while (*string != '\0')
    {
        print_char(*string, color);
        ++string;
    }
}

void VGA_DRIVER_report(const char* message, VGA_Driver_report_status status)
{
    print_char('[', VGA_DRIVER_MAGENTA_ON_BLACK);
    if (status == VGA_DRIVER_SUCCESS)
    {
        print_string(" SUCCESS ", 0x02);
    }
    else if (status == VGA_DRIVER_FAILURE)
    {
        print_string(" FAILURE ", 0x04);
    }
    print_string("] ", VGA_DRIVER_MAGENTA_ON_BLACK);
    print_string(message, VGA_DRIVER_MAGENTA_ON_BLACK);
    print_char('\n', VGA_DRIVER_MAGENTA_ON_BLACK);

}

void VGA_DRIVER_printf(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
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
                    print_string(s, VGA_DRIVER_MAGENTA_ON_BLACK);
                    break;
                case 'c':
                    c = (char)va_arg(ap, int);
                    print_char(c, VGA_DRIVER_MAGENTA_ON_BLACK);
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
            print_char(current, VGA_DRIVER_MAGENTA_ON_BLACK);
        }
        ++format;
    }
}

void VGA_DRIVER_init(const VGA_DRIVER_settings_t* settings)
{
    line = settings->initial_line;
    if (settings->should_copy_vga_buffer)
    {
        memcpy(scroll_buffer, buffer_address, VGA_DRIVER_SIZE);
    }
}
