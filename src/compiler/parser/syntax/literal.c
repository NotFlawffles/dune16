#include "compiler/parser/syntax/literal.h"

#define X(variant) #variant

char *literal_type_format(const enum literal_type_t this) {
    static char *literal_type_list_stringified[] = { LITERAL_TYPE_LIST };
    return literal_type_list_stringified[this];
}

#undef X
