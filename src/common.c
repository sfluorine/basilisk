#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

NORETURN void error_and_die(const char* fmt, ...) {
    fprintf(stderr, "ERROR: ");

    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
}
