#include <string.h>

#include "backend/dune16.h"
#include "compiler/generator.h"
#include "backend/common/define/flag_instruction.h"
#include "backend/common/register.h"

DYNC_DYNARR_IMPLEMENT(GenerationChunk, generation_chunk, u16);

DYNC_DYNMAP_IMPLEMENT(Opcode, opcode, const char *, u16);
DYNC_DYNMAP_IMPLEMENT(Flag, flag, char, u16);

bool dynarr_generation_chunk_compare_value_current(u16 value, u16 current) {
    return value == current;
}

char *dynarr_generation_chunk_element_format(u16 *const value) {
    return dync_format("%d", *value);
}

bool dynmap_opcode_compare_key_current(const char *key, const char *current) {
    return memcmp(key, current, strlen(key)) == 0;
}

char *dynarr_opcode_dynmap_key_element_format(const char **const value) {
    return dync_format("\"%s\"", *value);
}

char *dynarr_opcode_dynmap_value_element_format(u16 *const value) {
    return dync_format("%d", *value);
}

bool dynmap_flag_compare_key_current(char key, char current) {
    return key == current;
}

char *dynarr_flag_dynmap_key_element_format(char *const value) {
    return dync_format("\'%c\'", *value);
}

char *dynarr_flag_dynmap_value_element_format(u16 *const value) {
    return dync_format("%d", *value);
}

struct generator_t generator_new(const DynArrSyntax tree, char *const output_path) {
    DynMapOpcode opcodes = dynmap_opcode_new();

    /* opcode_t from `backend/instruction/instruction.h' is not being found here, i don't know what's the deal */
    enum {
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
    };

    dynmap_opcode_set(&opcodes, "nop", OPCODE_NOP);
    dynmap_opcode_set(&opcodes, "mov", OPCODE_MOV);
    dynmap_opcode_set(&opcodes, "str", OPCODE_STR);
    dynmap_opcode_set(&opcodes, "lod", OPCODE_LOD);
    dynmap_opcode_set(&opcodes, "add", OPCODE_ADD);
    dynmap_opcode_set(&opcodes, "sub", OPCODE_SUB);
    dynmap_opcode_set(&opcodes, "mul", OPCODE_MUL);
    dynmap_opcode_set(&opcodes, "mod", OPCODE_MOD);
    dynmap_opcode_set(&opcodes, "and", OPCODE_AND);
    dynmap_opcode_set(&opcodes, "ior", OPCODE_IOR);
    dynmap_opcode_set(&opcodes, "xor", OPCODE_XOR);
    dynmap_opcode_set(&opcodes, "not", OPCODE_NOT);
    dynmap_opcode_set(&opcodes, "lsl", OPCODE_LSL);
    dynmap_opcode_set(&opcodes, "lsr", OPCODE_LSR);
    dynmap_opcode_set(&opcodes, "cmp", OPCODE_CMP);
    dynmap_opcode_set(&opcodes, "bch", OPCODE_BCH);
    dynmap_opcode_set(&opcodes, "sys", OPCODE_SYS);
    dynmap_opcode_set(&opcodes, "hlt", OPCODE_HLT);

    DynMapOpcode registers = dynmap_opcode_new();

    dynmap_opcode_set(&registers, "a", REGISTER_A);
    dynmap_opcode_set(&registers, "b", REGISTER_B);
    dynmap_opcode_set(&registers, "c", REGISTER_C);
    dynmap_opcode_set(&registers, "d", REGISTER_D);
    dynmap_opcode_set(&registers, "pc", REGISTER_PC);
    dynmap_opcode_set(&registers, "lr", REGISTER_LR);

    DynMapFlag flags = dynmap_flag_new();

    dynmap_flag_set(&flags, 'z', FLAG_INSTRUCTION_ZERO);
    dynmap_flag_set(&flags, 'c', FLAG_INSTRUCTION_CARRY);
    dynmap_flag_set(&flags, 'o', FLAG_INSTRUCTION_OVERFLOW);

    return (struct generator_t) { dynarr_generation_chunk_new(), opcodes, registers, dynmap_opcode_new(), flags, tree, output_path };
}

static u8 generator_get_syntax_chunk_size(struct generator_t * const this, struct syntax_t *const syntax) {
    DynMapInstructionSize instruction_sizes = dune16_new().instruction_sizes;
    return dynmap_instruction_size_get(&instruction_sizes, dynmap_opcode_get(&this->opcodes, syntax->value.instruction.mnemonic));
}

void generator_generate(struct generator_t *const this) {
    DynArrGenerationChunk temporary_chunk = dynarr_generation_chunk_new();

    for (usize i = 0; i < this->tree.size; ++i) {
	if (this->tree.elements[i].type == SYNTAX_TYPE_LABEL) {
	    dynmap_opcode_set(&this->labels, this->tree.elements[i].value.label, temporary_chunk.size);
	} else {
	    const u8 current = generator_get_syntax_chunk_size(this, &this->tree.elements[i]);

	    for (usize j = 0; j < current; ++j) {
		dynarr_generation_chunk_push(&temporary_chunk, 0);
	    }
	}
    }

    for (usize i = 0; i < this->tree.size; ++i) {
	const DynArrGenerationChunk current = generator_generate_next(this, &this->tree.elements[i]);

	for (usize j = 0; j < current.size; ++j) {
	    dynarr_generation_chunk_push(&this->generation, current.elements[j]);
	}
    }

    FILE *file = fopen(this->output_path, "wb");

    fwrite(this->generation.elements, sizeof(u16), this->generation.size, file);
    fclose(file);
}

DynArrGenerationChunk generator_generate_next(struct generator_t *const this, struct syntax_t *const syntax) {
    switch (syntax->type) {
	case SYNTAX_TYPE_INSTRUCTION:
	    return generator_generate_instruction(this, syntax);

	case SYNTAX_TYPE_LABEL:
	    return dynarr_generation_chunk_new();

	default:
	    dync_panic("unreachable");
	    return dynarr_generation_chunk_new();
    }
}

DynArrGenerationChunk generator_generate_instruction(struct generator_t *const this, struct syntax_t *const syntax) {
    DynArrGenerationChunk chunk = dynarr_generation_chunk_new();

    const struct syntax_instruction_t *instruction = &syntax->value.instruction;

    dynarr_generation_chunk_push(&chunk, dynmap_opcode_get(&this->opcodes, instruction->mnemonic));

    const bool IMMEDIATE_BIT = instruction->arguments.has_source && instruction->arguments.b.type != SOURCE_ARGUMENT_TYPE_NAME;
    dynarr_generation_chunk_push(&chunk, dynmap_flag_get(&this->flags, instruction->flag) | IMMEDIATE_BIT);

    if (instruction->arguments.a) {
	if (memcmp(instruction->mnemonic, "bch", 3) == 0) {
	    chunk.elements[1] |= FLAG_INSTRUCTION_IMMEDIATE;
	    dynarr_generation_chunk_push(&chunk, dynmap_opcode_get(&this->labels, instruction->arguments.a));
	}

	dynarr_generation_chunk_push(&chunk, dynmap_opcode_get(&this->registers, instruction->arguments.a));
    }

    if (instruction->arguments.has_source) {
	switch (instruction->arguments.b.type) {
	    case SOURCE_ARGUMENT_TYPE_NAME:
		if (dynarr_opcode_dynmap_key_contains(&this->registers.keys, instruction->arguments.b.value.name)) {
		    dynarr_generation_chunk_push(&chunk, dynmap_opcode_get(&this->registers, instruction->arguments.b.value.name));
		} else if (dynarr_opcode_dynmap_key_contains(&this->labels.keys, instruction->arguments.b.value.name)) {
		    chunk.elements[1] |= FLAG_INSTRUCTION_IMMEDIATE;
		    dynarr_generation_chunk_push(&chunk, dynmap_opcode_get(&this->labels, instruction->arguments.b.value.name));
		} else {
		    dync_panic(dync_format("name `%s\' not found", instruction->arguments.b.value.name));
		}

		break;

	    case SOURCE_ARGUMENT_TYPE_INTEGER:
		dynarr_generation_chunk_push(&chunk, instruction->arguments.b.value.integer);
		break;

	    default:
		dync_panic("unreachable");
		break;
	}
    }

    return chunk;
}

DynArrGenerationChunk generator_generate_label(struct generator_t *const this, struct syntax_t *const syntax) {
    dynmap_opcode_set(&this->labels, syntax->value.label, this->generation.size);
    return dynarr_generation_chunk_new();
}
