
.global inw
.global outw
.global delay

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