#include <gdt.h>
#include <types.h>
#include <vmm.h>

#define STRIP_FLAGS(limit_top_plus_flags) ((limit_top_plus_flags) & 0b1111)
#define APPEND_FLAGS(stripped, flags) ((stripped) | (flags))
#define IS_EXECUTABLE(descriptor) (((descriptor->access_byte) & 0b1000) != 0) // simply checking the executable bit in the access byte

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

typedef struct
{
    word limit_bottom;
    word base_bottom;
    byte base_middle;
    byte access_byte;
    byte limit_top_plus_flags;
    byte base_top;
} __attribute__((packed)) GDT_descriptor_t;

// this is a 6-byte struct, using packed to prevent padding
typedef struct
{
    word size;
    GDT_descriptor_t* descriptors;
} __attribute__((packed)) GDT_header_t;

static GDT_descriptor_t gdt[3] =
{
    (GDT_descriptor_t) {0},
    (GDT_descriptor_t)
    {
        .limit_bottom = 0xFFFF,
        .base_bottom = 0x0000, // ignored
        .base_middle = 0x00, // ignored
        .access_byte = 0b10010010,
        .limit_top_plus_flags = 0b11000000 | 0b00001111,
        .base_top = 0x00 // ignored
    },
    (GDT_descriptor_t)
    {
        .limit_bottom = 0xFFFF,
        .base_bottom = 0x0000, // ignored
        .base_middle = 0x00, // ignored
        .access_byte = 0b10011010,
        .limit_top_plus_flags = 0b10100000 | 0b00001111,
        .base_top = 0x00 // ignored
    }
};

static GDT_header_t gdt_header =
{
    .descriptors = gdt,
    .size = sizeof(gdt) - 1
};

extern void gdt_load(const GDT_header_t* gdt_header);

void gdt_setup(void)
{
    gdt_load(&gdt_header);
}
