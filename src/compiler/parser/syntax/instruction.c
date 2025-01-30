#include "compiler/parser/syntax/syntax_instruction.h"

#define X(variant) #variant

char *source_argument_type_format(const enum source_argument_type_t this) {
    static char *source_argument_type_list_stringified[] = { SOURCE_ARGUMENT_TYPE_LIST };
    return source_argument_type_list_stringified[this];
}

#undef X
