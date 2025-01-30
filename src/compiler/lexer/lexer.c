#include <ctype.h>

#include "compiler/lexer/lexer.h"

DYNC_DYNARR_IMPLEMENT(Token, token, struct token_t);

bool dynarr_token_compare_value_current(struct token_t value, struct token_t current) {
    return false;
}

char *dynarr_token_element_format(struct token_t *const value) {
    return dync_format("(struct token_t) { .type = %s, .index = %ld, .value = %s }", token_type_format(value->type), value->index, option_dynarr_char_format(&value->value));
}

struct lexer_t lexer_new(char *const content) {
    return (struct lexer_t) { content, 0, *content };
}

DynArrToken lexer_tokenize(struct lexer_t *const this) {
    DynArrToken tokens = dynarr_token_new();

    while (this->current) {
	dynarr_token_push(&tokens, lexer_tokenize_next(this));
    }

    return tokens;
}

struct token_t lexer_tokenize_next(struct lexer_t *const this) {
    switch (lexer_skip_whitespace(this)) {
	case ',':
	    return lexer_advance_with_token(this, (struct token_t) { TOKEN_TYPE_COMMA, this->index, option_dynarr_char_none() });

	case ':':
	    return lexer_advance_with_token(this, (struct token_t) { TOKEN_TYPE_COLON, this->index, option_dynarr_char_none() });

	case '\0':
	    return (struct token_t) { TOKEN_TYPE_END_OF_FILE, this->index, option_dynarr_char_none() };

	default:
	    if (isalpha(this->current) || this->current == '_') return lexer_tokenize_identifier(this);
	    else if (isdigit(this->current)) return lexer_tokenize_decimal(this);

	    else {
		dync_panic(dync_format("unhandled character: `%c'", this->current));
		return (struct token_t) {};
	    }
    }
}

struct token_t lexer_tokenize_identifier(struct lexer_t *const this) {
    const usize index = this->index;
    DynArrChar value = dynarr_char_new();

    while (isalnum(this->current) || this->current == '_') {
	dynarr_char_push(&value, this->current);
	lexer_advance(this);
    }

    value.elements[value.size] = '\0';

    return (struct token_t) { TOKEN_TYPE_IDENTIFIER, index, option_dynarr_char_some(value) };
}

struct token_t lexer_tokenize_decimal(struct lexer_t *const this) {
    const usize index = this->index;
    DynArrChar value = dynarr_char_new();

    while (isdigit(this->current)) {
	dynarr_char_push(&value, this->current);
	lexer_advance(this);
    }

    value.elements[value.size] = '\0';

    return (struct token_t) { TOKEN_TYPE_DECIMAL, index, option_dynarr_char_some(value) };
}

struct token_t lexer_advance_with_token(struct lexer_t *const this, const struct token_t token) {
    lexer_advance(this);
    return token;
}

char lexer_skip_whitespace(struct lexer_t *const this) {
    while (isspace(this->current)) {
	lexer_advance(this);
    }

    return this->current;
}

char lexer_advance(struct lexer_t *const this) {
    return this->current = this->content[++this->index];
}
