#pragma once

#include <cj50/gen/Option.h>
#include <cj50/gen/ref.h>

static UNUSED
bool equal_int(const int *a, const int *b) {
    return *a == *b;
}

static UNUSED
bool equal_move_int(const int a, const int b) {
    return a == b;
}

static UNUSED
void drop_int(const int UNUSED a) {}


static UNUSED
int print_int(const int *n) {
    return printf("%i", *n);
}

static UNUSED
int print_move_int(int n) {
    return print_int(&n);
}

static UNUSED
int print_debug_int(const int *n) {
    return print_int(n);
}

static UNUSED
int print_debug_move_int(int n) {
    return print_debug_int(&n);
}


GENERATE_Option(int);
GENERATE_ref(int);
GENERATE_Option(ref(int));
