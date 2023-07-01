
.global inw
.global outw
.global delay
.global memset
.global memcpy
.global memmove
.global memcmp

delay:
    subs x0, x0, #1 //x0 = x0 - 1
    bne delay
    ret

outw:
    str w1, [x0] //store data w1 at addr preset in x0 
    ret

inw:
    ldr w0, [x0] //stores data at w0 taken from addr present in x0
    ret

memset:
    cmp x2, #0 
    beq memset_end

set:
    strb w1, [x0], #1 //store w1 data at addr present in x0 and increment x0 by 1
    subs x2, x2, #1 //x2 = x2-1
    bne set

memset_end:
    ret

memcmp:
    mov x3, x0
    mov x0, #0      //value return when both values are equal

compare:
    cmp x2, #0
    beq memcmp_end

    ldrb w4, [x3], #1 //get 4 byte data from addr at x3 and increment x3 by 1
    ldrb w5, [x1], #1 //get 4 byte data from addr at x1 and increment x1 by 1
    sub x2, x2, #1
    cmp w4, w5        //compare both value if equal compare next val else return 1
    beq compare

    mov x0, #1      //value return when both values are not equal

memcmp_end:
    ret

memmove:
memcpy:
    cmp x2, #0
    beq memcpy_end

    mov x4, #1

    cmp x1, x0      
    bhs copy        //copy in forward direction if dst addr is greater than source
    add x3, x1, x2  //x3 = x1+x2
    cmp x3, x0      
    bls copy        //if x3 < x2, then copy in forward direction

overlap:            //label to start copy in reverse direction
    sub x3, x2, #1
    add x0, x0, x3  //x0 = x0 + x2 - 1
    add x1, x1, x3  //x1 = x1 + x2 -1
    neg x4, x4      //x4 = -x4, this helps in decrementing addr with using same instruction set copy

copy:
    ldrb w3, [x1]
    strb w3, [x0]
    add x0, x0, x4
    add x1, x1, x4

    subs x2, x2, #1
    bne copy

memcpy_end:
    ret