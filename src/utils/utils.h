#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

/*delay function*/
void delay(uint64_t value);

/*write 4 bytes data to an addr*/
void outw(uint64_t addr, uint32_t value);

/*read 4 bytes data from an addr*/
uint32_t inw(uint64_t addr);

#endif