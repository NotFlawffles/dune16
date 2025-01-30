#ifndef DUNE16_GENERATOR_H
#define DUNE16_GENERATOR_H

#include "parser/parser.h"

DYNC_DYNARR_DECLARE(GenerationChunk, generation_chunk, u16);

DYNC_DYNMAP_DECLARE(Opcode, opcode, const char *, u16);
DYNC_DYNMAP_DECLARE(Flag, flag, char, u16);

struct generator_t {
    DynArrGenerationChunk generation;
    DynMapOpcode opcodes, registers, labels;
    DynMapFlag flags;
    DynArrSyntax tree;
    char *output_path;
};

struct generator_t generator_new(const DynArrSyntax tree, char *const output_path);

void generator_generate(struct generator_t *const this);

DynArrGenerationChunk generator_generate_next(struct generator_t *const this, struct syntax_t *const syntax);
DynArrGenerationChunk generator_generate_instruction(struct generator_t *const this, struct syntax_t *const syntax);
DynArrGenerationChunk generator_generate_label(struct generator_t *const this, struct syntax_t *const syntax);

#endif // DUNE16_GENERATOR_H
