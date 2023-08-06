#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>

#define KERNEL_BASE 0xffff000000000000

#define P2V(p) ((uint64_t)(p) + KERNEL_BASE)
#define V2P(v) ((uint64_t)(v) - KERNEL_BASE)

#endif