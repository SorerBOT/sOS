#include <console_io.h>
#include <vga_driver.h>
#include <stdarg.h>

void console_io_printf(const char * restrict format, ...)
{
    va_list ap;
    va_start(ap, format);

    vga_driver_vprintf_colored(VGA_DRIVER_COLOR_DEFAULT, format, ap);

    va_end(ap);
}
