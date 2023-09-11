#pragma once

#include <cj50/gen/Option.h>
#include <cj50/gen/ref.h>

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
int print_double(const double *x) {
    return printf("%.15g", *x);
}

static UNUSED
int print_move_double(double x) {
    return print_double(&x);
}

static UNUSED
int print_debug_double(const double *x) {
    // Why different from print_double?
    return printf("%g", *x);
}

static UNUSED
int print_debug_move_double(double x) {
    return print_debug_double(&x);
}

GENERATE_Option(double);
GENERATE_ref(double);
GENERATE_Option(ref(double));
