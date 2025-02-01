#include <string.h>
#include <stdlib.h>

#include "compiler/parser/parser.h"

DYNC_DYNARR_IMPLEMENT(Syntax, syntax, struct syntax_t);

DYNC_DYNMAP_IMPLEMENT(InstructionArgumentCount, instruction_argument_count, const char *, u8);

bool dynarr_syntax_compare_value_current(struct syntax_t value, struct syntax_t current) {
    return false;
}

char *dynarr_syntax_element_format(struct syntax_t *const value) {
    return dync_format("(struct syntax_t) { .type = %s, .index = %ld, .value = ... }", syntax_type_format(value->type), value->index);
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

	case TOKEN_TYPE_AMPERSAT:
	    return parser_parse_preprocessor(this);

	default:
	    dync_panic(dync_format("expected statement"));
	    return (struct syntax_t) {};
    }
}

struct syntax_t parser_parse_name(struct parser_t *const this) {
    if (dynarr_string_contains(&this->mnemonics, this->current.value.value.elements) && this->tokens.elements[this->index + 1].type != TOKEN_TYPE_COLON) {
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
	    instruction.arguments.b = parser_parse_literal(this);
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

struct syntax_t parser_parse_preprocessor(struct parser_t *const this) {
    const usize index = this->current.index;
    const enum syntax_preprocessor_type_t type = syntax_preprocessor_type_from_string(parser_advance(this).value.value.elements);
    parser_eat(this, TOKEN_TYPE_IDENTIFIER);

    /* TODO: parse other types of preprocessors; other than only (@raw and @res) */

    parser_eat(this, TOKEN_TYPE_LEFT_PARENTHESIS);
    DynArrString arguments = dynarr_string_new();

    while (this->current.type != TOKEN_TYPE_RIGHT_PARENTHESIS) {
	dynarr_string_push(&arguments, this->current.value.value.elements);
	parser_eat(this, TOKEN_TYPE_IDENTIFIER);

	if (this->current.type != TOKEN_TYPE_RIGHT_PARENTHESIS) {
	    parser_eat(this, TOKEN_TYPE_COMMA);
	}
    }

    parser_eat(this, TOKEN_TYPE_RIGHT_PARENTHESIS);

    DynArrLiteral literals = dynarr_literal_new();
    
    while (true) {
	const struct literal_t current = parser_parse_literal(this);

	dynarr_literal_push(&literals, current);

	if (this->current.type != TOKEN_TYPE_COMMA) {
	    break;
	}

	parser_advance(this);
    }

    return (struct syntax_t) {
	SYNTAX_TYPE_PREPROCESSOR,
	index,
	
	.value.preprocessor = {
	    type,
	    arguments,
	    .context_arguments.literals = literals,
	}
    };
}

struct literal_t parser_parse_literal(struct parser_t *const this) {
    struct token_t target = this->current;
    parser_advance(this);

    switch (target.type) {
	case TOKEN_TYPE_IDENTIFIER:
	    return (struct literal_t) { LITERAL_TYPE_NAME, .value.name = target.value.value.elements };

	case TOKEN_TYPE_DECIMAL:
	    return (struct literal_t) { LITERAL_TYPE_INTEGER, .value.integer = strtol(target.value.value.elements, NULL, 10) };

	case TOKEN_TYPE_CHARACTER:
	    return (struct literal_t) { LITERAL_TYPE_CHARACTER, .value.character = *target.value.value.elements };

	case TOKEN_TYPE_STRING:
	    return (struct literal_t) { LITERAL_TYPE_STRING, .value.string = target.value.value.elements };

	default:
	    dync_panic(dync_format("unexpected literal: %s", dynarr_token_element_format(&target)));
	    return (struct literal_t) {};
    }
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
