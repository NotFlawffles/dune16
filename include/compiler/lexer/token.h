#ifndef DUNE16_TOKEN_H
#define DUNE16_TOKEN_H

#include <dync/dynarr.h>
#include <dync/option.h>
#include <dync/common/types.h>

DYNC_DYNARR_DECLARE(Char, char, char);
DYNC_OPTION_DECLARE(DynArrChar, dynarr_char, DynArrChar);

#define TOKEN_TYPE_LIST \
    X(TOKEN_TYPE_IDENTIFIER), \
    X(TOKEN_TYPE_DECIMAL), \
    X(TOKEN_TYPE_CHARACTER), \
    X(TOKEN_TYPE_STRING), \
    X(TOKEN_TYPE_COMMA), \
    X(TOKEN_TYPE_COLON), \
    X(TOKEN_TYPE_AMPERSAT), \
    X(TOKEN_TYPE_LEFT_PARENTHESIS), \
    X(TOKEN_TYPE_RIGHT_PARENTHESIS), \
    X(TOKEN_TYPE_END_OF_FILE),

#define X(variant) variant

enum token_type_t { TOKEN_TYPE_LIST };

#undef X

char *token_type_format(const enum token_type_t this);

struct token_t {
    enum token_type_t type;
    usize index;
    OptionDynArrChar value;
};

#endif // DUNE16_TOKEN_H
