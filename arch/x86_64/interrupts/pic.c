#include <types.h>
#include <cpu_io.h>
#include "include/pic.h"
#include "include/idt.h"

#define PIC_MASTER_COMMAND  0x20
#define PIC_MASTER_DATA     0x21

#define PIC_SLAVE_COMMAND   0xA0
#define PIC_SLAVE_DATA      0xA1

#define PIC_MASTER_IRQ_MASK_FOR_SLAVE   0b00000100  // IRQ 2
#define PIC_SLAVE_IRQ_NUMBER_FOR_MASTER 0x02        // IRQ 2

#define PIC_X86_MODE 0x01

#define PIC_EOI_COMMAND_CODE 0x20

static inline void write_byte_plus_wait(word port, byte value);
static inline void pic_get_imrs(byte* imr_master, byte* imr_slave);
static inline void pic_set_imrs(byte imr_master, byte imr_slave);

static inline void write_byte_plus_wait(word port, byte value)
{
    cpu_io_write_byte(port, value);
    cpu_io_wait();
}
static inline void pic_get_imrs(byte* imr_master, byte* imr_slave)
{
    *imr_master = cpu_io_read_byte(PIC_MASTER_DATA);
    *imr_slave = cpu_io_read_byte(PIC_SLAVE_DATA);
}

static inline void pic_set_imrs(byte imr_master, byte imr_slave)
{
    write_byte_plus_wait(PIC_MASTER_DATA, imr_master);
    write_byte_plus_wait(PIC_SLAVE_DATA, imr_slave);
}

void pic_remap(void)
{
    byte imr_master;
    byte imr_slave;

    pic_get_imrs(&imr_master, &imr_slave);

    /* telling the PICs we're going to remap them */
    write_byte_plus_wait(PIC_MASTER_COMMAND, 0x11);
    write_byte_plus_wait(PIC_SLAVE_COMMAND, 0x11);

    /* telling the PICs what is their IDT offset */
    write_byte_plus_wait(PIC_MASTER_DATA, IDT_OFFSET_PIC_MASTER);
    write_byte_plus_wait(PIC_SLAVE_DATA, IDT_OFFSET_PIC_SLAVE);

    /* connecting the master & slave PICs */
    write_byte_plus_wait(PIC_MASTER_DATA, PIC_MASTER_IRQ_MASK_FOR_SLAVE);
    write_byte_plus_wait(PIC_SLAVE_DATA, PIC_SLAVE_IRQ_NUMBER_FOR_MASTER);

    /* telling the master PIC to work in x86 mode */
    write_byte_plus_wait(PIC_MASTER_DATA, PIC_X86_MODE);
    write_byte_plus_wait(PIC_SLAVE_DATA, PIC_X86_MODE);
    
    pic_set_imrs(0x00, 0x00);
}

void pic_send_EOI(uint8_t irq_number)
{
    if ( irq_number >= 8 )
    {
        write_byte_plus_wait(PIC_SLAVE_COMMAND, PIC_EOI_COMMAND_CODE);
    }
    cpu_io_write_byte(PIC_MASTER_COMMAND, PIC_EOI_COMMAND_CODE);
}
