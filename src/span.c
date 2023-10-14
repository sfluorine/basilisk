#include "span.h"

Span span_make(const char* data, int size) {
    return (Span) {
        .data = data,
        .size = size,
    };
}

Span span_from_cstr(const char* data) {
    int size = 0;
    while (data[size])
        size++;

    return (Span) {
        .data = data,
        .size = size,
    };
}

bool span_equals(Span lhs, Span rhs) {
    if (lhs.size != rhs.size)
        return false;

    for (int i = 0; i < lhs.size; i++) {
        if (lhs.data[i] != rhs.data[i]) {
            return false;
        }
    }

    return true;
}
