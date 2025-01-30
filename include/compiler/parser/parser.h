#ifndef DUNE16_PARSER_H
#define DUNE16_PARSER_H

#include <dync/dynmap.h>

#include "compiler/lexer/lexer.h"
#include "syntax/syntax.h"

DYNC_DYNARR_DECLARE(String, string, const char *);
DYNC_DYNARR_DECLARE(Syntax, syntax, struct syntax_t);

DYNC_DYNMAP_DECLARE(InstructionArgumentCount, instruction_argument_count, const char *, u8);

struct parser_t {
    DynArrString mnemonics;
    DynMapInstructionArgumentCount instruction_argument_counts;
    DynArrToken tokens;
    usize index;
    struct token_t current;
};

struct parser_t parser_new(const DynArrToken tokens);

DynArrSyntax parser_parse(struct parser_t *const this);

struct syntax_t parser_parse_next(struct parser_t *const this);
struct syntax_t parser_parse_name(struct parser_t *const this);
struct syntax_t parser_parse_instruction(struct parser_t *const this);
struct syntax_t parser_parse_label(struct parser_t *const this);

struct token_t parser_eat(struct parser_t *const this, const enum token_type_t type);
struct token_t parser_advance(struct parser_t *const this);

#endif // DUNE16_PARSER_H
