#include <sys/stat.h>
#include <malloc.h>
#include <string.h>

#include <dync/common/methods.h>

#include "compiler/io.h"

char *io_read_entire_file(char *const path) {
    struct stat stat_buffer;

    if (stat(path, &stat_buffer) == -1) {
	dync_panic(dync_format("Failed to read file: `%s\': No such file or directory.", path));
    }

    FILE *file = fopen(path, "r");

    char *content = calloc(stat_buffer.st_size + 1, sizeof(char));

    fread(content, sizeof(char), stat_buffer.st_size, file);
    fclose(file);

    content[strlen(content)] = '\0';
    return content;
}
