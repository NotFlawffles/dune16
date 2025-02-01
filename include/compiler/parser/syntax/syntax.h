#ifndef DUNE16_SYNTAX_H
#define DUNE16_SYNTAX_H

#include <dync/common/types.h>

#include "syntax_instruction.h"
#include "syntax_preprocessor.h"

#define SYNTAX_TYPE_LIST \
    X(SYNTAX_TYPE_INSTRUCTION), \
    X(SYNTAX_TYPE_LABEL), \
    X(SYNTAX_TYPE_PREPROCESSOR)

#define X(variant) variant

enum syntax_type_t { SYNTAX_TYPE_LIST };

#undef X

char *syntax_type_format(const enum syntax_type_t this);

union syntax_value_t {
    struct syntax_instruction_t instruction;
    char *label;
    struct syntax_preprocessor_t preprocessor;
};

struct syntax_t {
    enum syntax_type_t type;
    usize index;
    union syntax_value_t value;
};

#endif // DUNE16_SYNTAX_H
