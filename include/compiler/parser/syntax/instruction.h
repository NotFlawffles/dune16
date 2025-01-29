#ifndef DUNE16_INSTRUCTION_H
#define DUNE16_INSTRUCTION_H

#include <dync/common/types.h>

#define SOURCE_ARGUMENT_TYPE_LIST \
    X(SOURCE_ARGUMENT_TYPE_NAME), \
    X(SOURCE_ARGUMENT_TYPE_INTEGER),

#define X(variant) variant

enum source_argument_type_t { SOURCE_ARGUMENT_TYPE_LIST };

#undef X

char *source_argument_type_format(const enum source_argument_type_t this);

struct source_argument_t {
    enum source_argument_type_t type;

    union {
	char *name;
	usize integer;
    } value;
};

struct instruction_t {
    char *mnemonic, flag;

    struct {
	char *a;
	struct source_argument_t b;

	bool has_source;
    } arguments;
};

#endif // DUNE16_INSTRUCTION_H
