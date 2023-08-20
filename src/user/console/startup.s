.section .text
.global start

start:
    bl main
    bl exitu        //if program reaches end then call exit to put the process in killed list
end:
    b end
    