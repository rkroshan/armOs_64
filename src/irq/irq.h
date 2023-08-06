#ifndef __IRQ_H__
#define __IRQ_H__

#include <stdint.h>
#include "utils/memory.h"

/*For Raspi 3b*/
#define IO_BASE_ADDR P2V(0x3f000000)

/*Refer Interrupt Section Arm Peripheral 2837 doc*/
#define IRQ_BASIC_PENDING	(IO_BASE_ADDR + 0xB200)
#define ENABLE_IRQS_1		(IO_BASE_ADDR + 0xB210)
#define ENABLE_IRQS_2		(IO_BASE_ADDR + 0xB214)
#define ENABLE_BASIC_IRQS	(IO_BASE_ADDR + 0xB218)
#define DISABLE_IRQS_1		(IO_BASE_ADDR + 0xB21C)
#define DISABLE_IRQS_2		(IO_BASE_ADDR + 0xB220)
#define DISABLE_BASIC_IRQS	(IO_BASE_ADDR + 0xB224)

/*functions*/
void init_interrupt_controller(void);
uint32_t get_irq_number(void);

#endif