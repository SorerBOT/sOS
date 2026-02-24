#ifndef PIC_H
#define PIC_H

#include <types.h>

void pic_remap(void);
void pic_send_EOI(uint8_t irq_number);


#endif /* PIC_H */
