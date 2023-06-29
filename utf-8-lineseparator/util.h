/*
  Copyright (C) 2021-2023 Christian Jaeger, <ch@christianjaeger.ch>
  Published under the terms of the MIT License, see the LICENSE file.
*/

#ifndef UTIL_H_
#define UTIL_H_

#include <stdlib.h>
#include <stdio.h>
#include "macro-util.h"


#define UNUSED __attribute__ ((unused))

#define NORETURN __attribute__((__noreturn__)) void

#define MAX2(a,b)                               \
    ((a) < (b) ? (b) : (a))

#define MAX3(a,b,c)                             \
    ((a) < (b) ? MAX2(b, c) : MAX2(a, c))


#define WARN_(msg, ...)                                         \
    fprintf(stderr, msg "\n", __VA_ARGS__)
#define DIE_(msg, ...)                                          \
    do { fprintf(stderr, msg "\n", __VA_ARGS__); abort(); } while(0)
#define WARN(msg)                                               \
    fprintf(stderr, "%s\n", msg)
#define DIE(msg)                                                \
    do { fprintf(stderr, "%s\n", msg); abort(); } while(0)


#define FOR_RANGE(T, var, from, to)                     \
    __typeof__ (to) __for_range_to = (to);              \
    for (T var = (from); var < __for_range_to; var++)


#endif /* UTIL_H_ */
