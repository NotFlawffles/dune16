main:
    mov a, 69
    mov b, 42
    mov pc, swap
    hlt

swap:
    xor a, b
    xor b, a
    xor a, b
    hlt
