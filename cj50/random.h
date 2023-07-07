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
        ssize_t res = getrandom(&randnum, sizeof(randnum), 0);
        if (res < 0) {
            DIE_("getrandom: %s", strerror(errno));
        }
        if (sizeof(randnum) != res) {
            DIE_("getrandom: expected %lu bytes, got %li",
                 sizeof(randnum), res);
        }
        int ret = randnum & mask;
        // printf("randnum=%u, mask=%u, ret=%i\n", randnum, mask, ret);
        if (ret < range) {
            return ret;
        }
    }
}

// CAUTION: These are probably suboptimal (distribution of bits)!
// Consider them a HACK.

/// Get a random `double` value between 0. (inclusive) and 1. (exclusive).
double random_double() {
    uint64_t randnum;
    ssize_t res = getrandom(&randnum, sizeof(randnum), 0);
    if (res < 0) {
        DIE_("getrandom: %s", strerror(errno));
    }
    if (sizeof(randnum) != res) {
        DIE_("getrandom: expected %lu bytes, got %li",
             sizeof(randnum), res);
    }
    return randnum / 18446744073709551616.;
}

/// Get a random `float` value between 0. (inclusive) and 1. (exclusive).
float random_float() {
    return random_double();
}

