#include <string.h>

#include "compiler/parser/syntax/syntax_preprocessor.h"

DYNC_DYNARR_IMPLEMENT(Literal, literal, struct literal_t);

DYNC_DYNMAP_IMPLEMENT(PreprocessorType, preprocessor_type, const char *, enum syntax_preprocessor_type_t);

bool dynarr_literal_compare_value_current(struct literal_t value, struct literal_t current) {
    return false;
}

char *dynarr_literal_element_format(struct literal_t *const value) {
    return dync_format("(struct literal_t) { .type = %s, .value = ... }", literal_type_format(value->type));
}

bool dynmap_preprocessor_type_compare_key_current(const char *key, const char *current) {
    const usize LENGTH = strlen(key);
    return LENGTH == strlen(current) && memcmp(key, current, LENGTH) == 0;
}

char *dynarr_preprocessor_type_dynmap_key_element_format(const char **const value) {
    return dync_format("\"%s\"", *value);
}

char *dynarr_preprocessor_type_dynmap_value_element_format(enum syntax_preprocessor_type_t *const value) {
    return syntax_preprocessor_type_format(*value);
}

enum syntax_preprocessor_type_t syntax_preprocessor_type_from_string(char *value) {
    static DynMapPreprocessorType *syntax_preprocessor_type_map;

    if (!syntax_preprocessor_type_map) {
	syntax_preprocessor_type_map = malloc(sizeof(DynMapPreprocessorType));
	*syntax_preprocessor_type_map = dynmap_preprocessor_type_new();

	dynmap_preprocessor_type_set(syntax_preprocessor_type_map, "raw", SYNTAX_PREPROCESSOR_TYPE_RAW);
	dynmap_preprocessor_type_set(syntax_preprocessor_type_map, "res", SYNTAX_PREPROCESSOR_TYPE_RES);
    }

    return dynmap_preprocessor_type_get(syntax_preprocessor_type_map, value);
}

#define X(variant) #variant

char *syntax_preprocessor_type_format(const enum syntax_preprocessor_type_t this) {
    static char *syntax_preprocessor_type_list_stringified[] = { SYNTAX_PREPROCESSOR_TYPE_LIST };
    return syntax_preprocessor_type_list_stringified[this];
}

#undef X
