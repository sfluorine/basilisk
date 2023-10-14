#pragma once

#define SPAN_FMT "%.*s"
#define SPAN_ARG(span) span.size, span.data

#include <stdbool.h>

typedef struct {
    const char* data;
    int size;
} Span;

Span span_make(const char* data, int size);
Span span_from_cstr(const char* data);

bool span_equals(Span lhs, Span rhs);
