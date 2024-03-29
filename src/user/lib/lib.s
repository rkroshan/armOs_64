.section .text
.global memset
.global memcpy
.global memmove

memset:
    cmp x2, #0 
    beq memset_end

set:
    strb w1, [x0], #1 //store w1 data at addr present in x0 and increment x0 by 1
    subs x2, x2, #1 //x2 = x2-1
    bne set

memset_end:
    ret

memmove:
memcpy:
memcpy_fast:
    cmp x2, #0      //if z2 = size is zero then return
    beq memcpy_end
    mov x6, x0      //backup dst pointer
    mov x7, x1      //backup src pointer
    mov x4, #1      //to do 1 byte shift
    mov x5, #4      //to do 4 byte shift

    cmp x1, x0      
    bhs copy                //copy in forward direction if dst addr is greater than source
    add x3, x1, x2          //x3 = x1+x2
    cmp x3, x0      
    bls copy                //if x3 < x2, then copy in forward direction
overlap:                    //label to start copy in reverse direction
    sub x3, x2, #1
    add x0, x0, x3          //x0 = x0 + x2 - 1
    add x1, x1, x3          //x1 = x1 + x2 -1
    neg x4, x4              //x4 = -x4, this helps in decrementing addr with using same instruction set copy
    neg x5, x5              //x5 = -x5
copy:
memcpy_cmp_size:            //we will check if size x2 < 4, if less we will copy one byte at a time else 4 byte
    cmp x2, 4
    blo copy_1
copy_4:                     //load and store 4 bytes at a time from src to destination
    ldr w3, [x1]
    str w3, [x0]
    add x0, x0, x5
    add x1, x1, x5
    subs x2, x2, #4         //x2 = x2-4
    beq memcpy_restore_p    //if x2 becomes zero then restore the pointers and end
    b copy                  //else again loop to copy
copy_1:                     //load and store 1 byte at a time
    ldrb w3, [x1]
    strb w3, [x0]
    add x0, x0, x4
    add x1, x1, x4
    subs x2, x2, #1 //x2 = x2-1
    bne copy_1      //if x2 !=0, then again copy one byte
memcpy_restore_p:
    mov x0, x6      //restore dst pointer
    mov x1, x7      //restore src pointer
memcpy_end:
    ret