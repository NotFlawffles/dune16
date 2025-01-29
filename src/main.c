#include <stdio.h>
#include <string.h>

#include <dync/common/methods.h>

#include "compiler/generator.h"
#include "compiler/io.h"
#include "compiler/lexer/lexer.h"
#include "compiler/parser/parser.h"

#include "backend/dune16.h"

int usage(void) {
    fputs("Usage: dune16 [OPTIONS] INPUTS\n", stderr);
    fputs("\n", stderr);
    fputs("Options:\n", stderr);
    fputs("	com[pile] INPUTS\n", stderr);
    fputs("	exe[cute] INPUTS\n", stderr);
    fputs("\n", stderr);

    return 1;
}

int main(const int argc, char *const argv[]) {
    if (argc < 3) {
	return usage();
    }

    if (memcmp(argv[1], "compile", 3) == 0) {
	char *content = io_read_entire_file(argv[2]);
	struct lexer_t lexer = lexer_new(content);
	DynArrToken tokens = lexer_tokenize(&lexer);
	struct parser_t parser = parser_new(tokens);
	DynArrSyntax tree = parser_parse(&parser);
	struct generator_t generator = generator_new(tree, "out.dune");
	generator_generate(&generator);
    } else if (memcmp(argv[1], "execute", 3) == 0) {
	struct dune16_t dune16 = dune16_new();
	dune16_load_program_from_file(&dune16, argv[2]);
	dune16_run(&dune16);
	
	return dune16.registers.a;
    }
}
