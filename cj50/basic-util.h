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

#define MAX2(a, b)                              \
    ((a) < (b) ? (b) : (a))

#define MAX3(a, b, c)                           \
    ((a) < (b) ? MAX2(b, c) : MAX2(a, c))


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


#define VAR(v) __auto_type v
