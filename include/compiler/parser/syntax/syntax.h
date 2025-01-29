#ifndef DUNE16_SYNTAX_H
#define DUNE16_SYNTAX_H

#include <dync/common/types.h>

#include "instruction.h"

#define SYNTAX_TYPE_LIST \
    X(SYNTAX_TYPE_INSTRUCTION),

#define X(variant) variant

enum syntax_type_t { SYNTAX_TYPE_LIST };

#undef X

char *syntax_type_format(const enum syntax_type_t this);

struct syntax_value_t {
    struct instruction_t instruction;
};

struct syntax_t {
    enum syntax_type_t type;
    usize index;
    struct syntax_value_t value;
};

#endif // DUNE16_SYNTAX_H
