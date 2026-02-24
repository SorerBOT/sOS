#include <types.h>
#include <cpu_io.h>
#include "include/pic.h"

#define PIC_MASTER_COMMAND  0x20
#define PIC_MASTER_DATA     0x21

#define PIC_SLAVE_COMMAND   0xA0
#define PIC_SLAVE_DATA      0xA1

#define PIC_MASTER_IDT_OFFSET 0x20
#define PIC_SLAVE_IDT_OFFSET 0x28

#define PIC_MASTER_IRQ_MASK_FOR_SLAVE   0b00000100  // IRQ 2
#define PIC_SLAVE_IRQ_NUMBER_FOR_MASTER 0x02        // IRQ 2

#define PIC_X86_MODE 0x01

static inline void outb_plus_wait(word port, byte value);
static inline void pic_get_imrs(byte* imr_master, byte* imr_slave);
static inline void pic_set_imrs(byte imr_master, byte imr_slave);

static inline void outb_plus_wait(word port, byte value)
{
    cpu_io_outb(port, value);
    cpu_io_wait();
}
static inline void pic_get_imrs(byte* imr_master, byte* imr_slave)
{
    *imr_master = cpu_io_inb(PIC_MASTER_DATA);
    *imr_slave = cpu_io_inb(PIC_SLAVE_DATA);
}

static inline void pic_set_imrs(byte imr_master, byte imr_slave)
{
    outb_plus_wait(PIC_MASTER_DATA, imr_master);
    outb_plus_wait(PIC_SLAVE_DATA, imr_slave);
}

void pic_remap(void)
{
    byte imr_master;
    byte imr_slave;

    pic_get_imrs(&imr_master, &imr_slave);

    /* telling the PICs we're going to remap them */
    outb_plus_wait(PIC_MASTER_COMMAND, 0x11);
    outb_plus_wait(PIC_SLAVE_COMMAND, 0x11);

    /* telling the PICs what is their IDT offset */
    outb_plus_wait(PIC_MASTER_DATA, PIC_MASTER_IDT_OFFSET);
    outb_plus_wait(PIC_SLAVE_DATA, PIC_SLAVE_IDT_OFFSET);

    /* connecting the master & slave PICs */
    outb_plus_wait(PIC_MASTER_DATA, PIC_MASTER_IRQ_MASK_FOR_SLAVE);
    outb_plus_wait(PIC_SLAVE_DATA, PIC_SLAVE_IRQ_NUMBER_FOR_MASTER);

    /* telling the master PIC to work in x86 mode */
    outb_plus_wait(PIC_MASTER_DATA, PIC_X86_MODE);
    outb_plus_wait(PIC_SLAVE_DATA, PIC_X86_MODE);
    
    pic_set_imrs(imr_master, imr_slave);    
}
