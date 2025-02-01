main:
    sub		sp, 3
    str		sp, 108
    add		sp, 1
    str		sp, 10
    sub		sp, 2
    mov		a, 2
    mov		b, 1
    mov		c, sp
    mov		d, 3
    sys
    mov		b, 69
    bch		exit

exit:
    mov		a, 1
    sys
