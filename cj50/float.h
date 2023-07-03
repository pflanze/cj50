#pragma once

#include <cj50/gen/Option.h>

static UNUSED
void drop_float(float UNUSED x) { }

static UNUSED
bool equal_float(const float *a, const float *b) {
    return *a == *b;
}

static UNUSED
int print_debug_float(float x) {
    return printf("%g", x);
}

GENERATE_Option(float);

