/*
  Copyright (C) 2021-2023 Christian Jaeger, <ch@christianjaeger.ch>
  Published under the terms of the MIT License, see the LICENSE file.
*/

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include "macro-util.h"


#define UNUSED __attribute__ ((unused))

#define NORETURN __attribute__((__noreturn__)) void

#define RESTRICT restrict

#undef MIN

/// Return the smaller of the two arguments.
#define MIN(a, b)                                                       \
    ({                                                                  \
        __typeof__ (a) HYGIENIC(_a) = (a);                              \
        __typeof__ (b) HYGIENIC(_b) = (b);                              \
        HYGIENIC(_a) < HYGIENIC(_b) ? HYGIENIC(_a) : HYGIENIC(_b);      \
    })

#undef MAX

/// Return the larger of the two arguments.
#define MAX(a, b)                                                       \
    ({                                                                  \
        __typeof__ (a) HYGIENIC(_a) = (a);                              \
        __typeof__ (b) HYGIENIC(_b) = (b);                              \
        HYGIENIC(_a) > HYGIENIC(_b) ? HYGIENIC(_a) : HYGIENIC(_b);      \
    })

/// Return the smallest of the 3 arguments.
#define MIN3(a, b, c)                           \
    MIN(MIN((a), (b)), (c))

/// Return the largest of the 3 arguments.
#define MAX3(a, b, c)                           \
    MAX(MAX((a), (b)), (c))


/// Print a warning to standard error. Does not take values to embed
/// in `msg`.
#define WARN(msg)                                               \
    fprintf(stderr, "%s\n", msg)

/// Print a warning to standard error. `msg` is a format string, the
/// values to embed are expected afterwards.
#define WARN_(msg, ...)                                         \
    fprintf(stderr, msg "\n", __VA_ARGS__)

/// Print a message to standard error, then abort. Does not take
/// values to embed in `msg`.
#define DIE(msg)                                                \
    do {                                                        \
        fprintf(stderr, "%s\n", msg);                           \
        fflush(stderr);                                         \
        abort();                                                \
    } while(0)

/// Print a message to standard error, then abort. `msg` is a format
/// string, the values to embed are expected afterwards.
#define DIE_(msg, ...)                                          \
    do {                                                        \
        fprintf(stderr, msg "\n", __VA_ARGS__);                 \
        fflush(stderr);                                         \
        abort();                                                \
    } while(0)

#define FOR_RANGE(T, var, from, to)                     \
    __typeof__ (to) __for_range_to = (to);              \
    for (T var = (from); var < __for_range_to; var++)


#define AUTO __auto_type
