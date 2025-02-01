#include <stdio.h>
#include <unistd.h>

#include "backend/common/define/syscall.h"
#include "backend/instruction/callback.h"
#include "backend/dune16.h"
#include "backend/common/define/flag_dune16.h"
#include "backend/common/define/flag_instruction.h"

static u16 callback_get_source(struct dune16_t *const dune16) {
    return dune16->instruction.flags & FLAG_INSTRUCTION_IMMEDIATE ? dune16->instruction.arguments.b : *dune16_get_register(dune16, dune16->instruction.arguments.b);
}

void callback_nop(struct dune16_t *const dune16) {}

void callback_mov(struct dune16_t *const dune16) {
    *dune16_get_register(dune16, dune16->instruction.arguments.a) = callback_get_source(dune16);
}

void callback_str(struct dune16_t *const dune16) {
    dune16_write_16(dune16, *dune16_get_register(dune16, dune16->instruction.arguments.a), callback_get_source(dune16));
}

void callback_lod(struct dune16_t *const dune16) {
    *dune16_get_register(dune16, dune16->instruction.arguments.a) = dune16_read_16(dune16, callback_get_source(dune16));
}

void callback_add(struct dune16_t *const dune16) {
    u16 *destination = dune16_get_register(dune16, dune16->instruction.arguments.a);
    *destination = *destination + callback_get_source(dune16);
}

void callback_sub(struct dune16_t *const dune16) {
    u16 *destination = dune16_get_register(dune16, dune16->instruction.arguments.a);
    *destination = *destination - callback_get_source(dune16);
}

void callback_mul(struct dune16_t *const dune16) {
    u16 *destination = dune16_get_register(dune16, dune16->instruction.arguments.a);
    *destination = *destination * callback_get_source(dune16);
}

void callback_mod(struct dune16_t *const dune16) {
    u16 *destination = dune16_get_register(dune16, dune16->instruction.arguments.a);
    *destination = *destination % callback_get_source(dune16);
}

void callback_and(struct dune16_t *const dune16) {
    u16 *destination = dune16_get_register(dune16, dune16->instruction.arguments.a);
    *destination = *destination & callback_get_source(dune16);
}

void callback_ior(struct dune16_t *const dune16) {
    u16 *destination = dune16_get_register(dune16, dune16->instruction.arguments.a);
    *destination = *destination | callback_get_source(dune16);
}

void callback_xor(struct dune16_t *const dune16) {
    u16 *destination = dune16_get_register(dune16, dune16->instruction.arguments.a);
    *destination = *destination ^ callback_get_source(dune16);
}

void callback_not(struct dune16_t *const dune16) {
    u16 *destination = dune16_get_register(dune16, dune16->instruction.arguments.a);
    *destination = ~*destination;
}

void callback_lsl(struct dune16_t *const dune16) {
    u16 *destination = dune16_get_register(dune16, dune16->instruction.arguments.a);
    *destination = *destination << callback_get_source(dune16);
}

void callback_lsr(struct dune16_t *const dune16) {
    u16 *destination = dune16_get_register(dune16, dune16->instruction.arguments.a);
    *destination = *destination >> callback_get_source(dune16);
}

void callback_cmp(struct dune16_t *const dune16) {
    const i16 RESULT = *dune16_get_register(dune16, dune16->instruction.arguments.a) - callback_get_source(dune16);

    dune16->flags &= ~FLAG_DUNE16_CONDITIONAL_FLAGS;
    dune16->flags |= (RESULT == 0) << 1;
    dune16->flags |= (RESULT > 0) << 2;
    dune16->flags |= (RESULT < 0) << 3;
}

void callback_bch(struct dune16_t *const dune16) {
    dune16->registers.lr = dune16->registers.pc + 1;
    dune16->registers.pc = dune16->instruction.arguments.a;
}

void callback_sys(struct dune16_t *const dune16) {
    switch (dune16->registers.a) {
	case SYSCALL_RESET:
	    break;

	case SYSCALL_EXIT:
	    dune16->flags ^= FLAG_DUNE16_RUNNING;
	    break;

	case SYSCALL_WRITE: {
	    const u16 file_descriptor = dune16->registers.b;
	    const u8 *destination = (u8 *) &dune16->memory[dune16->registers.c];
	    const u16 size = dune16->registers.d;

	    write(file_descriptor, destination, size);
	    break;
	}

	case SYSCALL_READ: {
	    const u16 file_descriptor = dune16->registers.b;
	    u8 *destination = (u8 *) &dune16->memory[dune16->registers.c];
	    const u16 size = dune16->registers.d;

	    read(file_descriptor, destination, size);
	    break;
	}
    }
}
