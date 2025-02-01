#ifndef DUNE16_SYNTAX_PREPROCESSOR_H
#define DUNE16_SYNTAX_PREPROCESSOR_H

#include <dync/dynmap.h>

#include "common/dync.h"
#include "compiler/parser/syntax/literal.h"

DYNC_DYNARR_DECLARE(Literal, literal, struct literal_t);

#define SYNTAX_PREPROCESSOR_TYPE_LIST \
    X(SYNTAX_PREPROCESSOR_TYPE_RAW), \
    X(SYNTAX_PREPROCESSOR_TYPE_RES),

#define X(variant) variant

enum syntax_preprocessor_type_t { SYNTAX_PREPROCESSOR_TYPE_LIST };

#undef X

DYNC_DYNMAP_DECLARE(PreprocessorType, preprocessor_type, const char *, enum syntax_preprocessor_type_t);

enum syntax_preprocessor_type_t syntax_preprocessor_type_from_string(char *value);

char *syntax_preprocessor_type_format(const enum syntax_preprocessor_type_t this);

union syntax_preprocessor_context_arguments_t {
    DynArrLiteral literals;
};

struct syntax_preprocessor_t {
    enum syntax_preprocessor_type_t type;
    DynArrString arguments;
    union syntax_preprocessor_context_arguments_t context_arguments;
};

#endif // DUNE16_SYNTAX_PREPROCESSOR_H
