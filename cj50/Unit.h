#pragma once

#include <stdbool.h>

/// The `Unit` type has exactly one value, `Unit()`, and is used when
/// there is no other meaningful value that could be returned. Note
/// that the C programming language also has `void`, but that can't be
/// made part of data structures, hence we use `Unit` instead in such
/// cases.

typedef struct Unit {
} Unit;

/// Construct the single possible value that `Unit` can represent.

#define Unit() \
    ((Unit) {})

static UNUSED
bool equal_Unit(UNUSED const Unit *a, UNUSED const Unit *b) {
    return true;
}

static UNUSED
int print_debug_Unit(UNUSED const Unit *a) {
    return printf("Unit()");
}
