main:
    bch store_greeting_message
    bch greet
    bch exit

store_greeting_message:
    mov a, 1000
    str a, 72
    add a, 1
    str a, 101
    add a, 1
    str a, 108
    add a, 1
    str a, 108
    add a, 1
    str a, 111
    add a, 1
    str a, 10
    mov pc, lr

greet:
    mov a, 2
    mov b, 1
    mov c, 1000
    mov d, 6
    sys
    mov pc, lr

exit:
    mov a, 1
    mov b, 69
    sys
