#include <stddef.h>

#include <types.h> 
#include <vga_driver.h>

#define STRIP_FLAGS(limit_top_plus_flags) (limit_top_plus_flags & 0b1111)
#define APPEND_FLAGS(stripped, flags) (stripped | flags)
#define IS_EXECUTABLE(descriptor) ((descriptor->access_byte & 0b1000) != 0) // simply checking the executable bit in the access byte

/*
 * 63         56 55  52 51      48 47        40 39          32
 * +------------+------+----------+------------+-------------+
 * |    Base    | Flags|  Limit   | Access Byte|    Base     |
 * |   31..24   | 3..0 |  19..16  |    7..0    |   23..16    |
 * +------------+------+----------+------------+-------------+
 * |  base_top  |flags/|limit_top |access_byte |base_middle  |  <- Struct Fields
 * +------------+------+----------+------------+-------------+
 * 31                         16 15                          0
 * +----------------------------+----------------------------+
 * |            Base            |           Limit            |
 * |           15..0            |           15..0            |
 * +----------------------------+----------------------------+
 * |         base_bottom        |        limit_bottom        |  <- Struct Fields
 * +----------------------------+----------------------------+
 */

typedef struct __attribute__((packed))
{
    word limit_bottom;
    word base_bottom;
    byte base_middle;
    byte access_byte;
    byte limit_top_plus_flags;
    byte base_top;
} GDT_descriptor_t;

// this is a 6-byte struct, using packed to prevent padding
typedef struct __attribute__((packed))
{
    word size;
    GDT_descriptor_t* descriptors;
} GDT_header_t;

extern GDT_header_t GDT_HEADER;

void update_gdt()
{
    size_t descriptors_count = (GDT_HEADER.size + 1) / sizeof(GDT_descriptor_t);
    byte long_mode_flags = 0b10100000; // Granularity ON, Size Flags: OFF, Long mode Flag: ON, Reserved: OFF.
    for (size_t i = 0; i < descriptors_count; ++i)
    {
        GDT_descriptor_t* descriptor = &GDT_HEADER.descriptors[i];

        if ( !IS_EXECUTABLE(descriptor) )
        {
            continue;
        }

        byte stripped = STRIP_FLAGS(descriptor->limit_top_plus_flags);
        byte with_new_flags = APPEND_FLAGS(stripped, long_mode_flags);
        descriptor->limit_top_plus_flags = with_new_flags;
    }

    VGA_DRIVER_report("updated GDT table...", VGA_DRIVER_SUCCESS);
}
