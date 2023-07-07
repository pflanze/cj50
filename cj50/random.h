#pragma once

//! Functions for getting good random numbers.

#include <sys/random.h>
#include <cj50/basic-util.h>

/// Get a random integer value between 0 (inclusive) and `range` (exclusive).
int random_int(int range) {
    // Note: this prohibits ever returning the MAXINT value.  Note:
    // this may block when run before the random number source has
    // entropy or is initialized.

    if (range < 1) {
        DIE_("random_int: argument not in nat range: %i", range);
    }
    unsigned int urange = range;
    assert(sizeof(urange) == 32/8); // necessary for __builtin_clz ?
    unsigned int nlz = __builtin_clz(urange);
    unsigned int mask = (1u << (32 - nlz)) - 1;

    unsigned int randnum;
    while (true) {
        ssize_t res = getrandom(&randnum, sizeof(unsigned int), 0);
        if (res < 0) {
            DIE_("getrandom: %s", strerror(errno));
        }
        if (sizeof(unsigned int) != res) {
            DIE_("getrandom: expected %lu bytes, got %li",
                 sizeof(unsigned int), res);
        }
        int ret = randnum & mask;
        // printf("randnum=%u, mask=%u, ret=%i\n", randnum, mask, ret);
        if (ret < range) {
            return ret;
        }
    }
}
