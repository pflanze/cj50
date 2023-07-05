/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  Published under the terms of the MIT License, see the LICENSE file.
*/

#pragma once

#include "String.h"
#include "macro-util.h"


#define Result(T) XCAT(Result_,T)

#define DEFTYPE_Result_(T)                                      \
    typedef struct {                                            \
        bool is_err;                                            \
        union {                                                 \
            String err;                                         \
            T ok;                                               \
        };                                                      \
    } Result(T);                                                \
                                                                \
    static inline UNUSED                                        \
    void XCAT(drop_, Result(T))(const Result(T) s) {            \
        if (s.is_err) {                                         \
            String_drop(s.err);                                 \
        }                                                       \
        /* We don't drop the .ok part here as that one */       \
        /* may have changed ownership in the mean time! */      \
    }                                                           \
                                                                \
    static UNUSED                                               \
    bool XCAT(equal_, Result(T))(const Result(T) *a,            \
                                 const Result(T) *b) {          \
        return ((a->is_err == b->is_err) &&                     \
                (a->is_err                                      \
                 ? String_equal(&a->err, &b->err)               \
                 : XCAT(T, _equal)(&a->ok, &b->ok)));           \
    }


#define Err(T, string)                          \
    (Result_(T)) { .is_err = true, .err = string }
#define Ok(T, val)                              \
    (Result_(T)) { .is_err = false, .ok = val }

#define Result_is_Ok(v) (!((v).is_err))
#define Result_is_Err(v) ((v).is_err)

// We don't release the .ok part here as that one may have changed
// ownership in the mean time!
// XX obsolete, use drop instead
#define result_drop(v)                          \
    if (Result_is_Err(v)) String_drop((v).err)

