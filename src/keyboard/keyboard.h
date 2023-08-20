#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <stdint.h>

#define KEYBOARD_BUFFER_MAX 500

struct KeyboardBuffer {
    char buffer[KEYBOARD_BUFFER_MAX]; 
    int front;/*read index*/
    int end; /*write index*/
    int size; /*current size*/
};

/*return key char from read index if buffer not empty*/
char read_key_buffer(void);
/*interrupt handler for key press*/
void keyboard_handler(void);

#endif