__asm__(".code32\n");

#define VGA_DRIVER_IMPLEMENTATION
#include "drivers/32-bit/vga_driver.h"

void bootloader_stage_3()
{
    VGA_DRIVER_report("[sOS]: stage 2 completed...", VGA_DRIVER_SUCCESS);
    VGA_DRIVER_report("[sOS]: successfully entered protected 32-bit mode.", VGA_DRIVER_SUCCESS);
    VGA_DRIVER_report("[sOS]: running stage 2...", VGA_DRIVER_SUCCESS);
    VGA_DRIVER_report("[sOS]: running stage 2...", VGA_DRIVER_FAILURE);
    VGA_DRIVER_printf("%s, %d, %c\n", "HELLO THERE", -123, 'z');
    VGA_DRIVER_printf("%s, %d, %c\n", "HELLO THERE", 0, 'z');
    VGA_DRIVER_printf("%d\n", "HELLO THERE", 1234567890123456789, 'z');

    // Hang forever
    while(1);
}
