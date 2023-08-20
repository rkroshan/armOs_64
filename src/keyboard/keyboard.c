#include "keyboard.h"
#include "process/process.h"
#include "uart/uart.h"

static struct KeyboardBuffer key_buffer = { {0}, 0, 0, KEYBOARD_BUFFER_MAX };

static void write_key_buffer(char ch)
{
    int front = key_buffer.front;
    int end = key_buffer.end;
    int size = key_buffer.size;

    if ((end + 1) % size == front) { /*it means this is the last position, if we fill in end=front which means empty so we cannot fill in*/
        return;
    }
    key_buffer.buffer[end++] = ch; /*add the val and update the write index*/
    key_buffer.end = end % size; /*wrap around write index*/
}

char read_key_buffer(void)
{
    int front = key_buffer.front; /*current read index*/

    if (front == key_buffer.end) { /*this means buffer is empty*/
        sleep(-2);       /*put the process to sleep for keyboard press, since this call will come from process via svc*/
    }
    
    key_buffer.front = (key_buffer.front + 1) % key_buffer.size; /*update the read index*/
    return key_buffer.buffer[front]; /*get the old index val*/
}

static char keyboard_read(void)
{
    return read_char(); /*read the char from UART RXBUF*/
}

void keyboard_handler(void)
{
    char ch = keyboard_read(); /*read the char from UART RXBUF*/

    write_key_buffer(ch); /*write the char into buffer*/
    wake_up(-2); /*wakeup any process waiting for keyinput, -2 represent processes waiting for keyboard press*/
}
