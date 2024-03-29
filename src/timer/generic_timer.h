#ifndef __GENERIC_TIMER_H__
#define __GENERIC_TIMER_H__

#include <stdint.h>
#include "utils/memory.h"

/*generic interrupt time base addr core 0 , cortex A7*/
#define CNTP_EL0	        (P2V(0x40000040))
#define CNTP_STATUS_EL0     (P2V(0x40000060))
 
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
/*return ticks per 10ms*/
uint64_t get_ticks(void); 

#endif