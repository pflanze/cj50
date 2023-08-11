#pragma once

#include <cj50/gen/Option.h>


static UNUSED
bool equal_size_t(const size_t *a, const size_t *b) {
    return *a == *b;
}

static UNUSED
bool equal_move_size_t(const size_t a, const size_t b) {
    return a == b;
}

static UNUSED
void drop_size_t(const size_t UNUSED a) {}

static UNUSED
int print_size_t(size_t n) {
    return printf("%zu", n);
}

static UNUSED
int print_debug_size_t(const size_t *n) {
    return print_size_t(*n);
}


GENERATE_Option(size_t);
