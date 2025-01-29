#include "backend/instruction/addresser.h"
#include "backend/dune16.h"

void addresser_implied(struct dune16_t *const dune16) {}

void addresser_a(struct dune16_t *const dune16) {
    dune16->instruction.arguments.a = dune16_read_16(dune16, dune16->registers.pc++);
}

void addresser_ab(struct dune16_t *const dune16) {
    *(u32 *) &dune16->instruction.arguments = dune16_read_32(dune16, dune16->registers.pc);
    dune16->registers.pc += 2;
}
