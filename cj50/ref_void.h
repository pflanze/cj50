#pragma once

#include <cj50/gen/ref.h>

GENERATE_ref(void);

static UNUSED
void drop_ref_void(UNUSED ref_void self) {}

static UNUSED
bool equal_ref_void(const ref_void *a, const ref_void *b) {
    return *a == *b;
}

static UNUSED
int print_debug_ref_void(const ref_void *self) {
    return printf("ref_void(%p)", *self);
}

