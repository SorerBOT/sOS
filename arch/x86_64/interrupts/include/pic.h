#ifndef PIC_H
#define PIC_H

#include <types.h>

#define PIC_IRQ_COUNT_PER_UNIT 8


void pic_remap(void);
void pic_send_EOI(uint8_t irq_number);


#endif /* PIC_H */
