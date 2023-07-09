#ifndef __GENERIC_TIMER_H__
#define __GENERIC_TIMER_H__

#include <stdint.h>

/*generic interrupt time base addr core 0 , cortex A7*/
#define CNTP_EL0	        (0x40000040)
#define CNTP_STATUS_EL0     (0x40000060)

#ifdef RASPI4
#define TIMER_BASE_ADDR     0xfe000000
#define TIMER_LOAD          (TIMER_BASE_ADDR + 0xB400)
#define TIMER_CTL           (TIMER_BASE_ADDR + 0xB408)
#define TIMER_ACK           (TIMER_BASE_ADDR + 0xB40C)
#define TIMER_MSKIRQ        (TIMER_BASE_ADDR + 0xB414)
#define TIMER_PREDIV        (TIMER_BASE_ADDR + 0xB41c)
#endif
 
void enable_timer(void);
uint32_t read_timer_status(void);
void set_timer_interval(uint32_t value);
uint32_t read_timer_freq(void);

/*initialize generic timer core 0 at the moment*/
void init_timer(void);
/*timer exception handler*/
void timer_interrupt_handler(void);

/*IRQ enable / disable*/
void enable_irq(void);
void disable_irq(void);

#endif