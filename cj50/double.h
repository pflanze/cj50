#pragma once

#include <cj50/gen/Option.h>

static UNUSED
void drop_double(double UNUSED x) { }

static UNUSED
bool equal_double(const double *a, const double *b) {
    return *a == *b;
}

static UNUSED
bool equal_move_double(const double a, const double b) {
    return a == b;
}

static UNUSED
int print_double(double x) {
    return printf("%.15g", x);
}

static UNUSED
int print_debug_double(double x) {
    return printf("%g", x);
}

GENERATE_Option(double);

