#ifndef DUNE16_LEXER_H
#define DUNE16_LEXER_H

#include <dync/dynarr.h>

#include "token.h"

DYNC_DYNARR_DECLARE(Token, token, struct token_t);

struct lexer_t {
    char *content;
    usize index;
    char current;
};

struct lexer_t lexer_new(char *const content);

DynArrToken lexer_tokenize(struct lexer_t *const this);

struct token_t lexer_tokenize_next(struct lexer_t *const this);
struct token_t lexer_tokenize_identifier(struct lexer_t *const this);
struct token_t lexer_tokenize_decimal(struct lexer_t *const this);
struct token_t lexer_advance_with_token(struct lexer_t *const this, const struct token_t token);

char lexer_skip_whitespace(struct lexer_t *const this);
char lexer_advance(struct lexer_t *const this);

#endif // DUNE16_LEXER_H
