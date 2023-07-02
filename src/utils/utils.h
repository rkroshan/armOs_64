#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include "print.h"

/*delay function*/
void delay(uint64_t value);

/*write 4 bytes data to an addr*/
void outw(uint64_t addr, uint32_t value);

/*read 4 bytes data from an addr*/
uint32_t inw(uint64_t addr);

/*memory operation functions*/
void memset(void *dst, int value, unsigned int size);
void memcpy(void *dst, void *src, unsigned int size);
void memmove(void *dst, void *src, unsigned int size);
int memcmp(void *src1, void *src2, unsigned int size);

/*system register read/write*/
/*give current el*/
uint8_t get_el(void);

#endif