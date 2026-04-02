ENTRY(kernel_start)
OUTPUT_FORMAT(binary)
STARTUP(bin/64-bit/asm/kernel/kernel_start.o)

KERNEL_BASE_ADDRESS_PHYSICAL = 0x10000;
KERNEL_BINARY_MAP_VIRTUAL_OFFSET = 0XFFFFFFFF80000000;

SECTIONS
{
    . = KERNEL_BASE_ADDRESS_PHYSICAL + KERNEL_BINARY_MAP_VIRTUAL_OFFSET;

    kernel_base_physical_address = KERNEL_BASE_ADDRESS_PHYSICAL;

    .text ALIGN(4k) :
    {
        *(.text*)
    }

    .rodata ALIGN(4k) :
    {
        *(.rodata*)
    }

    .data ALIGN(4k) :
    {
        *(.data*)
    }

    .bss ALIGN(4k) :
    {
        *(COMMON)
        *(.bss*)
    }

    kernel_end_physical_address = . - KERNEL_BINARY_MAP_VIRTUAL_OFFSET;
}
