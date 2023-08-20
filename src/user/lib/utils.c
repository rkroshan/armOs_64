#include "user/lib/lib.h"

void all_caps_char(char* buffer)
{
    while(*buffer != '\0')
    {
        char c = *buffer;

        if(c >= 'a' && c <= 'z'){
            *buffer = c - 32 ; /*offset to make small caps bigger*/
        }
        buffer++;
    }
    return;
}