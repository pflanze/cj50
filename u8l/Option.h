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
        bool is_some;                                           \
        T value;                                                \
    } Option(T);                                                \
                                                                \
    /* CAUTION: only call drop when value has not been moved */ \
    static inline UNUSED                                        \
    void XCAT(drop_, Option(T))(const Option(T) s) {            \
        if (s.is_some) {                                        \
            XCAT(drop_, T)(s.value);                            \
        }                                                       \
    }                                                           \
                                                                \
    static UNUSED                                               \
    bool XCAT(equal_, Option(T))(const Option(T) *a,            \
                                 const Option(T) *b) {          \
        return ((a->is_some == b->is_some) &&                   \
                (a->is_some ?                                   \
                 XCAT(equal_, T)(&a->value, &b->value) :        \
                 true));                                        \
    }                                                           \
                                                                \
    static UNUSED                                               \
    Option(T) XCAT(some_, T)(const T val) {                     \
        return (Option(T)) { .is_some = true, .value = val };   \
    }                                                           \
                                                                \
    static UNUSED                                               \
    Option(T) XCAT(none_, T)() {                                \
        return (Option(T)) { .is_some = false };                \
    }                                                           \
                                                                \
    static UNUSED                                               \
    T XCAT(unwrap_, Option(T))(const Option(T) s) {             \
        if (s.is_some) {                                        \
            return s.value;                                     \
        } else {                                                \
            DIE("unwrap: value is a none");                     \
        }                                                       \
    }                                                           \
                                                                \
    static UNUSED                                               \
    int XCAT(print_debug_, Option(T))(const Option(T) *s) {     \
        if (! s->is_some) {                                     \
            return printf("none()");                            \
        } else {                                                \
            int ret = 0;                                        \
            int res;                                            \
            res = printf("some(");                              \
            if (res < 0) { return res; }                        \
            ret += res;                                         \
            res = XCAT(print_debug_, T)(/* XX & */s->value);    \
            if (res < 0) { return res; }                        \
            ret += res;                                         \
            res = printf(")");                                  \
            if (res < 0) { return res; }                        \
            ret += res;                                         \
            return ret;                                         \
        }                                                       \
    }                                                           \
                                                                \
    /* This is a HACK to allow print_debug */                   \
    /* to accept arguments without & */                         \
    static UNUSED                                               \
    int XCAT(move_print_debug_, Option(T))(const Option(T) s) { \
        return XCAT(print_debug_, Option(T))(&s);               \
    }



#define NONE { .is_some = false }


#endif /* OPTION_H_ */
