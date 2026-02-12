#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H

#include <types.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum
{
    VGA_DRIVER_SUCCESS,
    VGA_DRIVER_FAILURE
} vga_driver_report_status;

typedef struct
{
    size_t initial_line;
    bool should_copy_vga_buffer;
} vga_driver_settings_t;

void vga_driver_report(const char* message, vga_driver_report_status status);
void vga_driver_printf(const char* format, ...);
void vga_driver_clear();
void vga_driver_print_blue_screen(const char* format, ...);
void vga_driver_init(const vga_driver_settings_t* settings);


#endif /* VGA_DRIVER_H */
