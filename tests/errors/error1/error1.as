
; Warning: Trying to declare a label for an entry statement
; Error: label 'PL' isn't defined anywhere in the file
LABEL2: .entry PL

LABEL: mov r1, r2

; Warning: Trying to declare a label for an extern statement
; Error: LABEL already defined
LABEL: .extern W

; Error: invalid addressing mode for source parameter
mov 15r, r1

; Error: invalid addressing mode for destination parameter
mov r1, 2

; Error: Invalid number of arguments
cmp

; Error: Invalid number of arguments
stop 5

; Error: label ABC isn't defined anywhere in the soruce code
jmp ABC(r1,r2)
