#pragma once

#include <cj50/gen/ref.h>

static UNUSED
int print_debug_void(const void *self) {
    return printf("ref_void(%p)", self);
}

GENERATE_ref(void);

