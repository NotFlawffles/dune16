#include "compiler/parser/syntax/syntax.h"

#define X(variant) #variant

char *syntax_type_format(const enum syntax_type_t this) {
    static char *syntax_type_list_stringified[] = { SYNTAX_TYPE_LIST };
    return syntax_type_list_stringified[this];
}

#undef X
