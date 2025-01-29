#include "compiler/lexer/token.h"

DYNC_DYNARR_IMPLEMENT(Char, char, char);
DYNC_OPTION_IMPLEMENT(DynArrChar, dynarr_char, DynArrChar);

bool dynarr_char_compare_value_current(char value, char current) {
    return value == current;
}

char *dynarr_char_element_format(char *const value) {
    return dync_format("%c", *value);
}

char *option_dynarr_char_value_format(DynArrChar *const value) {
    return dynarr_char_format(value);
}

#define X(variant) #variant

char *token_type_format(const enum token_type_t this) {
    static char *token_type_list_stringified[] = { TOKEN_TYPE_LIST };
    return token_type_list_stringified[this];
}

#undef X
