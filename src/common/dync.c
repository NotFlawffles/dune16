#include <string.h>

#include "common/dync.h"

DYNC_DYNARR_IMPLEMENT(String, string, const char *);

bool dynarr_string_compare_value_current(const char *value, const char *current) {
    return memcmp(value, current, 3) == 0;
}

char *dynarr_string_element_format(const char **const value) {
    return dync_format("\"%s\"", *value);
}
