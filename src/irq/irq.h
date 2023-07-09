#ifndef __IRQ_H__
#define __IRQ_H__

#include <stdint.h>

#ifdef RASPI3B
/*For Raspi 3b*/
#define IO_BASE_ADDR 0x3f000000

/*Refer Interrupt Section Arm Peripheral 2837 doc*/
#define IRQ_BASIC_PENDING	(IO_BASE_ADDR + 0xB200)
#define ENABLE_IRQS_1		(IO_BASE_ADDR + 0xB210)
#define ENABLE_IRQS_2		(IO_BASE_ADDR + 0xB214)
#define ENABLE_BASIC_IRQS	(IO_BASE_ADDR + 0xB218)
#define DISABLE_IRQS_1		(IO_BASE_ADDR + 0xB21C)
#define DISABLE_IRQS_2		(IO_BASE_ADDR + 0xB220)
#define DISABLE_BASIC_IRQS	(IO_BASE_ADDR + 0xB224)

#endif

#ifdef RASPI4
#define GIC_BASE  0xff840000

#define DIST                GIC_BASE + 0x1000
#define ICD_TYPE            DIST + 0x4
#define ICD_ISENABLE        DIST + 0x100
#define ICD_ICENABLE        DIST + 0x180
#define ICD_SET_PENDING     DIST + 0x200
#define ICD_SET_ACTIVE      DIST + 0x300
#define ICD_PR              DIST + 0x400
#define ICD_PTR             DIST + 0x800
#define ICD_GROUP           DIST + 0x80
#define ICD_ICFGR           DIST + 0xc00

#define CPU_INTERFACE       GIC_BASE + 0x2000
#define ICC_PR              CPU_INTERFACE + 0x4
#define ICC_ACK             CPU_INTERFACE + 0xc
#define ICC_EOI             CPU_INTERFACE + 0x10

#endif
/*functions*/
void init_interrupt_controller(void);
uint32_t get_irq_number(void);

#endif