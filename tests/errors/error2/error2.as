; Error: LABEL isn't delcared in the file
; Error: LABEL2 isn't declared in the file
mcr myMacro
    mov r3, LABEL
    prn LABEL2
endmcr

myMacro

; Error: invalid data format - x2
INVALIDDATA: .data 1,
INVALIDDATA2: .data "abcd"

; Error: invalid string format
INVALIDDATA3: .string "ab

; Error: label entry is not declared anywhere in the file
.entry abcde

; Error: invalid addressing mode for argument number (1)
inc -3



