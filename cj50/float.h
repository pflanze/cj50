#pragma once

#include <cj50/gen/Option.h>
#include <cj50/gen/ref.h>

static UNUSED
void drop_float(float UNUSED x) { }

static UNUSED
bool equal_float(const float *a, const float *b) {
    return *a == *b;
}

static UNUSED
bool equal_move_float(const float a, const float b) {
    return a == b;
}

static UNUSED
int print_float(const float *x) {
    return printf("%g", *x);
}

static UNUSED
int print_move_float(float x) {
    return print_float(&x);
}

static UNUSED
int print_debug_float(const float *x) {
    return print_float(x);
}

static UNUSED
int print_debug_move_float(float x) {
    return print_debug_float(&x);
}

GENERATE_Option(float);
GENERATE_ref(float);
GENERATE_Option(ref(float));
