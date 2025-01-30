#include <string.h>
#include <stdlib.h>

#include "compiler/parser/parser.h"

DYNC_DYNARR_IMPLEMENT(Syntax, syntax, struct syntax_t);
DYNC_DYNARR_IMPLEMENT(String, string, const char *);

DYNC_DYNMAP_IMPLEMENT(InstructionArgumentCount, instruction_argument_count, const char *, u8);

bool dynarr_syntax_compare_value_current(struct syntax_t value, struct syntax_t current) {
    return false;
}

char *dynarr_syntax_element_format(struct syntax_t *const value) {
    return dync_format("(struct syntax_t) { .type = SYNTAX_TYPE_INSTRUCTION, .index = %ld, .value = ... }", value->index);
}

bool dynarr_string_compare_value_current(const char *value, const char *current) {
    return memcmp(value, current, 3) == 0;
}

char *dynarr_string_element_format(const char **const value) {
    return dync_format("\"%s\"", *value);
}

bool dynmap_instruction_argument_count_compare_key_current(const char *lhs, const char *rhs) {
    return memcmp(lhs, rhs, strlen(lhs)) == 0;
}

char *dynarr_instruction_argument_count_dynmap_key_element_format(const char **const value) {
    return dync_format("\"%s\"", *value);
}

char *dynarr_instruction_argument_count_dynmap_value_element_format(u8 *const value) {
    return dync_format("%d", *value);
}

struct parser_t parser_new(const DynArrToken tokens) {
    DynArrString mnemonics = dynarr_string_new();

    dynarr_string_push(&mnemonics, "nop");
    dynarr_string_push(&mnemonics, "mov");
    dynarr_string_push(&mnemonics, "str");
    dynarr_string_push(&mnemonics, "lod");
    dynarr_string_push(&mnemonics, "add");
    dynarr_string_push(&mnemonics, "sub");
    dynarr_string_push(&mnemonics, "mul");
    dynarr_string_push(&mnemonics, "mod");
    dynarr_string_push(&mnemonics, "and");
    dynarr_string_push(&mnemonics, "ior");
    dynarr_string_push(&mnemonics, "xor");
    dynarr_string_push(&mnemonics, "not");
    dynarr_string_push(&mnemonics, "lsl");
    dynarr_string_push(&mnemonics, "lsr");
    dynarr_string_push(&mnemonics, "cmp");
    dynarr_string_push(&mnemonics, "bch");
    dynarr_string_push(&mnemonics, "sys");
    dynarr_string_push(&mnemonics, "hlt");

    DynMapInstructionArgumentCount instruction_argument_counts = dynmap_instruction_argument_count_new();

    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[0], 	0);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[1], 	2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[2], 	2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[3], 	2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[4], 	2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[5], 	2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[6], 	2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[7], 	2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[8], 	2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[9], 	2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[10], 2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[11], 1);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[12], 2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[13], 2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[14], 2);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[15], 1);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[16], 0);
    dynmap_instruction_argument_count_set(&instruction_argument_counts, mnemonics.elements[17], 0);

    return (struct parser_t) { mnemonics, instruction_argument_counts, tokens, 0, *tokens.elements };
}

DynArrSyntax parser_parse(struct parser_t *const this) {
    DynArrSyntax tree = dynarr_syntax_new();

    while (this->current.type != TOKEN_TYPE_END_OF_FILE) {
	dynarr_syntax_push(&tree, parser_parse_next(this));
    }

    return tree;
}

struct syntax_t parser_parse_next(struct parser_t *const this) {
    switch (this->current.type) {
	case TOKEN_TYPE_IDENTIFIER:
	    return parser_parse_name(this);

	default:
	    dync_panic(dync_format("expected statement"));
	    return (struct syntax_t) {};
    }
}

struct syntax_t parser_parse_name(struct parser_t *const this) {
    if (dynarr_string_contains(&this->mnemonics, this->current.value.value.elements)) {
	return parser_parse_instruction(this);
    } else {
	return parser_parse_label(this);
    }
}

struct syntax_t parser_parse_instruction(struct parser_t *const this) {
    const usize index = this->current.index;
    struct syntax_instruction_t instruction = { this->current.value.value.elements, this->current.value.value.elements[3] };
    instruction.mnemonic[3] = '\0';
    const u8 argument_count = dynmap_instruction_argument_count_get(&this->instruction_argument_counts, instruction.mnemonic);
    instruction.arguments.has_source = argument_count == 2;
    parser_advance(this);

    switch (dynmap_instruction_argument_count_get(&this->instruction_argument_counts, instruction.mnemonic)) {
	case 0:
	    break;

	case 1:
	    instruction.arguments.a = this->current.value.value.elements;
	    parser_eat(this, TOKEN_TYPE_IDENTIFIER);
	    break;

	case 2:
	    instruction.arguments.a = this->current.value.value.elements;
	    parser_eat(this, TOKEN_TYPE_IDENTIFIER); 
	    parser_eat(this, TOKEN_TYPE_COMMA);

	    switch (this->current.type) {
		case TOKEN_TYPE_IDENTIFIER:
		    instruction.arguments.b = (struct source_argument_t) { SOURCE_ARGUMENT_TYPE_NAME, .value.name = this->current.value.value.elements };
		    break;

		case TOKEN_TYPE_DECIMAL:
		    instruction.arguments.b = (struct source_argument_t) { SOURCE_ARGUMENT_TYPE_INTEGER, .value.integer = strtol(this->current.value.value.elements, NULL, 10) };
		    break;

		default:
		    dync_panic(dync_format("expected identifier or decimal in source argument for %s", instruction.mnemonic));
		    break;
	    }

	    parser_advance(this);
	    break;
    }

    return (struct syntax_t) { SYNTAX_TYPE_INSTRUCTION, index, .value.instruction = instruction };
}

struct syntax_t parser_parse_label(struct parser_t *const this) {
    const usize index = this->current.index;
    char *name = this->current.value.value.elements;
    parser_advance(this);
    parser_eat(this, TOKEN_TYPE_COLON);
    
    return (struct syntax_t) { SYNTAX_TYPE_LABEL, index, .value.label = name };
}

struct token_t parser_eat(struct parser_t *const this, const enum token_type_t type) {
    if (this->current.type != type) {
	dync_panic(dync_format("unexpected token: %s, expected: %s", token_type_format(this->current.type), token_type_format(type)));
    }

    return parser_advance(this);
}

struct token_t parser_advance(struct parser_t *const this) {
    return this->current = this->tokens.elements[++this->index];
}
