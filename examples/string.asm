main:
    mov		b, target_a
    bch		strlen
    mov		b, a
    bch		exit

strlen:
    mov		a, b
    xor		c, c

strlen_loop:
    lod		c, a
    cmp		c, 0
    subz	a, b
    mulz	a, 2
    movz	pc, lr
    add		a, 1
    mov		pc, strlen_loop

exit:
    mov		a, 1
    sys


target_a:
    @raw(byte)	"Hello, world!", 10, 0
