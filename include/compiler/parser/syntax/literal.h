#ifndef DUNE16_LITERAL_H
#define DUNE16_LITERAL_H

#include <dync/common/types.h>

#define LITERAL_TYPE_LIST \
    X(LITERAL_TYPE_NAME), \
    X(LITERAL_TYPE_INTEGER), \
    X(LITERAL_TYPE_CHARACTER), \
    X(LITERAL_TYPE_STRING),

#define X(variant) variant

enum literal_type_t { LITERAL_TYPE_LIST };

#undef X

char *literal_type_format(const enum literal_type_t this);

union literal_value_t {
    char *name;
    usize integer;
    char character;
    char *string;
};

struct literal_t {
    enum literal_type_t type;
    union literal_value_t value;
};

#endif // DUNE16_LITERAL_H
