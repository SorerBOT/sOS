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
#define VGA_DRIVER_WIDTH 80
#define VGA_DRIVER_HEIGHT 25
#define VGA_DRIVER_SHADOW_HEIGHT 40
#define VGA_DRIVER_MAX_CHARS (VGA_DRIVER_WIDTH * VGA_DRIVER_HEIGHT)
#define VGA_DRIVER_SHADOW_MAX_CHARS (VGA_DRIVER_WIDTH * VGA_DRIVER_SHADOW_HEIGHT)

typedef struct
{
    size_t initial_line;
    bool should_copy_vga_buffer;
} vga_driver_settings_t;

void vga_driver_init(const vga_driver_settings_t* settings);
void vga_driver_clear_colored(byte new_background_color);
void vga_driver_flush_shadow_buffer();

void vga_driver_print_string_colored(byte color, const char* string);
#endif /* VGA_DRIVER_H */
