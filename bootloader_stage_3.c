__asm__(".code32\n");

#define VGA_DRIVER_IMPLEMENTATION
#include "drivers/32-bit/vga_driver.h"

void bootloader_stage_3()
{
    VGA_DRIVER_report("[sOS]: stage 2 completed...", VGA_DRIVER_SUCCESS);
    VGA_DRIVER_report("[sOS]: successfully entered protected 32-bit mode.", VGA_DRIVER_SUCCESS);
    VGA_DRIVER_report("[sOS]: running stage 2...", VGA_DRIVER_SUCCESS);
    VGA_DRIVER_report("[sOS]: running stage 2...", VGA_DRIVER_FAILURE);

    // Hang forever
    while(1);
}
