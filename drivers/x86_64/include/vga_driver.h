#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H

#include <types.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#define VGA_DRIVER_COLOR_DEFAULT 0x07
#define VGA_DRIVER_COLOR_SUCCESS 0x02
#define VGA_DRIVER_COLOR_FAILURE 0x04
#define VGA_DRIVER_COLOR_BLUE_SCREEN 0x1F

#define VGA_DRIVER_PRINTF(format, ...) vga_driver_printf_colored(VGA_DRIVER_COLOR_DEFAULT, (format), ##__VA_ARGS__)

typedef struct
{
    size_t initial_line;
    bool should_copy_vga_buffer;
} vga_driver_settings_t;


void vga_driver_init(const vga_driver_settings_t* settings);
void vga_driver_vprintf_colored(byte color, const char* format, va_list ap);
void vga_driver_printf_colored(byte color, const char* format, ...);
void vga_driver_clear_colored(byte new_background_color);
void vga_driver_flush_shadow_buffer();

#endif /* VGA_DRIVER_H */
