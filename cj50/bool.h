#pragma once

// /usr/lib/llvm-13/lib/clang/13.0.1/include/stdbool.h uses a macro
// definition, why? Remove it or it creates bad names.
#undef bool
typedef _Bool bool;

static UNUSED
void drop_bool(const char UNUSED s) { }

static UNUSED
bool equal_bool(const bool *a, const bool *b) {
    return (!!*a) == (!!*b);
}

static UNUSED
bool equal_move_bool(const bool a, const bool b) {
    return (!!a) == (!!b);
}

static UNUSED
int print_debug_bool(bool v) {
    return printf("%s", v ? "true" : "false");
}


GENERATE_Option(bool);
