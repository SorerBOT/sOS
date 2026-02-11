#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H

#include <types.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum
{
    VGA_DRIVER_SUCCESS,
    VGA_DRIVER_FAILURE
} VGA_Driver_report_status;

typedef struct
{
    size_t initial_line;
    bool should_copy_vga_buffer;
} VGA_DRIVER_settings_t;

void VGA_DRIVER_report(const char* message, VGA_Driver_report_status status);
void VGA_DRIVER_printf(const char* format, ...);
void VGA_DRIVER_clear();
void VGA_DRIVER_blue_screen(const char* message);
void VGA_DRIVER_init(const VGA_DRIVER_settings_t* settings);


#endif /* VGA_DRIVER_H */
