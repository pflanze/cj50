/*
  Copyright (C) 2021-2023 Christian Jaeger, <ch@christianjaeger.ch>
  Published under the terms of the MIT License, see the LICENSE file.
*/

#ifndef OPTION_H_
#define OPTION_H_

#include <stdbool.h>
#include "basic-util.h"
#include "macro-util.h"


#define Option(T) XCAT(Option_,T)

#define DEFTYPE_Option(T)                                       \
    typedef struct Option(T) {                                  \
        bool is_none;                                           \
        T value;                                                \
    } Option(T);                                                \
                                                                \
    /* We don't drop the `value` as it may have */              \
    /* changed ownership in the mean time! */                   \
    static inline UNUSED                                        \
    void XCAT(drop_, Option(T))(const Option(T) UNUSED s) { }   \
                                                                \
    static UNUSED                                               \
    bool XCAT(equal_, Option(T))(const Option(T) *a,            \
                                 const Option(T) *b) {          \
        return ((a->is_none == b->is_none) &&                   \
                (a->is_none ? true                              \
                 : XCAT(equal_, T)(&a->value, &b->value)));     \
    }                                                           \
                                                                \
    static UNUSED                                               \
    bool XCAT(is_some_, T)(const Option(T) *s) {                \
        return !(s->is_none);                                   \
    }                                                           \
                                                                \
    static UNUSED                                               \
    bool XCAT(is_none_, T)(const Option(T) *s) {                \
        return s->is_none;                                      \
    }                                                           \
                                                                \
    static UNUSED                                               \
    Option(T) XCAT(some_, T)(T val) {                           \
        return (Option(T)) { .is_none = false, .value = val };  \
    }                                                           \
                                                                \
    static UNUSED                                               \
    Option(T) XCAT(none_, T)() {                                \
        return (Option(T)) { .is_none = false };                \
    }


#define NONE { .is_none = true }


#endif /* OPTION_H_ */
