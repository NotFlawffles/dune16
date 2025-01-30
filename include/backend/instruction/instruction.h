#ifndef DUNE16_INSTRUCTION_H
#define DUNE16_INSTRUCTION_H

#include <dync/common/types.h>

enum opcode_t {
    OPCODE_NOP,
    OPCODE_MOV,
    OPCODE_STR,
    OPCODE_LOD,
    OPCODE_ADD,
    OPCODE_SUB,
    OPCODE_MUL,
    OPCODE_MOD,
    OPCODE_AND,
    OPCODE_IOR,
    OPCODE_XOR,
    OPCODE_NOT,
    OPCODE_LSL,
    OPCODE_LSR,
    OPCODE_CMP,
    OPCODE_BCH,
    OPCODE_SYS,
    OPCODE_HLT,
    OPCODE_COUNT,
};

struct dune16_t;

struct instruction_t {
    enum opcode_t opcode;

    void (*addresser)(struct dune16_t *const dune16);
    void (*callback)(struct dune16_t *const dune16);

    u8 flags;

    struct {
	u16 a, b;
    } arguments;
};

#endif // DUNE16_INSTRUCTION_H
