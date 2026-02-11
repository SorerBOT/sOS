#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H

#ifndef VGA_DRIVER_INITIAL_LINE
#define VGA_DRIVER_INITIAL_LINE 13
#endif

typedef enum
{
    VGA_DRIVER_SUCCESS,
    VGA_DRIVER_FAILURE
} VGA_Driver_report_status;

void VGA_DRIVER_report(const char* message, VGA_Driver_report_status status);
void VGA_DRIVER_printf(const char* format, ...);
void VGA_DRIVER_clear();


#endif /* VGA_DRIVER_H */
