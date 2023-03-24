mov r1, r3
.extern W
add r1, r7
inc LABEL

mcr myMacro
    add r1, r2
    inc r3
    prn r7
endmcr

data1: .data 4, 5,8     ,11     , 9,2,3
data2: .string  "abcdefg h k l  o p:"

LABEL: prn r3

jmp LABEL(r1,#3)

prn W

.entry LABEL
.entry data1

myMacro
