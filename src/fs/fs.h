#ifndef __FS_H__
#define __FS_H__

#include "utils/memory.h"

#define FS_BASE P2V(0x30000000)

/*do basic FAT16 filesystem bootup signature validation whether FS is available or not*/
void init_fs(void);

#endif