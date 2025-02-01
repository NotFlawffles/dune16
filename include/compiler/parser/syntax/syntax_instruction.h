#ifndef DUNE16_SYNTAX_INSTRUCTION_H
#define DUNE16_SYNTAX_INSTRUCTION_H

#include <dync/common/types.h>

#include "compiler/parser/syntax/literal.h"

struct syntax_instruction_t {
    char *mnemonic, flag;

    struct {
	char *a;
	struct literal_t b;

	bool has_source;
    } arguments;
};

#endif // DUNE16_SYNTAX_INSTRUCTION_H
