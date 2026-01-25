__asm__(".code32\n");

#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define VGA_DRIVER_BUFFER_ADDRESS 0xB8000
#define VGA_DRIVER_WHITE_ON_BLACK 0x05
#define VGA_DRIVER_INITIAL_LINE 13
#define VGA_DRIVER_WIDTH 80
#define VGA_DRIVER_HEIGHT 25

typedef enum
{
    VGA_DRIVER_SUCCESS,
    VGA_DRIVER_FAILURE
} VGA_Driver_report_status;

void VGA_DRIVER_report(const char* message, VGA_Driver_report_status status);
void VGA_DRIVER_printf(const char* format, ...);
void VGA_DRIVER_clear();


#endif /* VGA_DRIVER_H */

#ifdef VGA_DRIVER_IMPLEMENTATION

static size_t line = VGA_DRIVER_INITIAL_LINE;
static size_t offset = 0;

static void print_char(char character, uint8_t color);
static void print_string(const char* string, uint8_t color);

static void print_char(char character, uint8_t color)
{
    if (character == '\n')
    {
        ++line;
        offset = 0;
        return;
    }
    volatile uint8_t* buffer_address = (volatile uint8_t*) VGA_DRIVER_BUFFER_ADDRESS;
    if (line >= VGA_DRIVER_HEIGHT && offset >= VGA_DRIVER_WIDTH)
    {
        // need to scroll
        return;
    }
    else
    {
        size_t index = (line * VGA_DRIVER_WIDTH + offset) * 2;
        buffer_address[index] = (uint8_t)character;
        buffer_address[index + 1] = color;
        ++offset;
    }

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
    print_char('[', VGA_DRIVER_WHITE_ON_BLACK);
    if (status == VGA_DRIVER_SUCCESS)
    {
        print_string(" SUCCESS ", 0x02);
    }
    else if (status == VGA_DRIVER_FAILURE)
    {
        print_string(" FAILURE ", 0x04);
    }
    print_string("] ", VGA_DRIVER_WHITE_ON_BLACK);
    print_string(message, VGA_DRIVER_WHITE_ON_BLACK);
    print_char('\n', VGA_DRIVER_WHITE_ON_BLACK);

}
void VGA_DRIVER_printf(const char* format, ...)
{

}

#endif /* VGA_DRIVER_IMPLEMENTATION */
