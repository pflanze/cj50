/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  Published under the terms of the MIT License, see the LICENSE file.
*/

#ifndef RESULT_H_
#define RESULT_H_

#include "String.h"
#include "macro-util.h"


#define Result_(T) XCAT(Result_,T)

#define DEFTYPE_Result_(T)                                      \
    typedef struct {                                            \
        bool is_err;                                            \
        union {                                                 \
            String err;                                         \
            T ok;                                               \
        };                                                      \
    } Result_(T);                                               \
                                                                \
    static inline UNUSED                                        \
    void XCAT(Result_(T),_drop)(const Result_(T) s) {           \
        if (s.is_err) {                                         \
            String_drop(s.err);                                 \
        }                                                       \
        /* We don't drop the .ok part here as that one */       \
        /* may have changed ownership in the mean time! */      \
    }                                                           \
                                                                \
    static UNUSED                                               \
    bool XCAT(Result_(T),_equal)(const Result_(T) *a,           \
                                 const Result_(T) *b) {         \
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

#define PROPAGATE_return(T, r)                                          \
    if (Result_is_Err(r)) {                                             \
        /* (r).err.needs_freeing = false;                               \
           after the next line, but usually deallocated anyway */       \
        return Err(T, (r).err);                                         \
    }


/*

  Label based cleanup handling:

    Result_Sometype foo() {
        BEGIN_PROPAGATE(Sometype);
        ...
        if (bar) RETURN_goto(l1, val1);
        Result_foo x = givingx();
        PROPAGATE_goto(l2, Sometype, x);
        RETURN(Ok(Sometype, val2));
     l2:
        result_drop(x);
     l1:
        cleanup1();
        END_PROPAGATE;
    }
  
*/

#define BEGIN_PROPAGATE(T)                      \
    Result_(T) __return;

#define RETURN_goto(label, val)                 \
    __return = val;                             \
    goto label;

#define RETURN(val)                             \
    __return = val;

#define END_PROPAGATE                           \
    return __return;

#define PROPAGATE_goto(label, T, r)                                     \
    if (Result_is_Err(r)) {                                             \
        __return = Err(T, (r).err);                                     \
        (r).err.needs_freeing = false;                                  \
        goto label;                                                     \
    }


/*

  Block based cleanup handling:


    BEGIN_PROPAGATE(Bar);

    if_Ok(Bar, result) {
        ....
        RETURN(Ok(Bar, { }));
    }
    // other cleanup than the result

    END_PROPAGATE;

  Or

    BEGIN_PROPAGATE(Bar);

    if_let_Ok(Bar, var, expr) {
        ....
        RETURN(Ok(Bar, { }));
    }
    // other cleanup than the result

    END_PROPAGATE;

*/

#define if_Ok(T, expr)                                  \
    __typeof__(expr) __if_Ok_result = expr;             \
    if (Result_is_Err(__if_Ok_result)) {                \
        RETURN(Err(T, __if_Ok_result.err));             \
        result_drop(__if_Ok_result);                    \
    } else 

// ^ XX Bummer: the whole if_Ok should automatically be wrapped in a
// new block, but can't (or does the MACRO{ } trick work here? TODO)

/*
#define if_let_Ok(T, var, expr)                         \
    __typeof__(expr) __if_Ok_result = expr;             \
    if (Result_is_Err(__if_Ok_result)) {                \
        RETURN(Err(T, __if_Ok_result.err));             \
        result_drop(__if_Ok_result);                    \
    } else {                                            \
    __typeof__(__if_Ok_result.ok) var =                 \
        __if_Ok_result.ok;                              \

The problem is that we'd need a ENDif_let_Ok to close off
the opening brace.

So...:
*/

#define if_let_Ok(Terr, var, expr)                      \
    __typeof__(expr) __if_Ok_result = expr;             \
    __typeof__(__if_Ok_result.ok) var =                 \
        __if_Ok_result.ok;                              \
    if (Result_is_Err(__if_Ok_result)) {                \
        RETURN(Err(Terr, __if_Ok_result.err));          \
        result_drop(__if_Ok_result);                    \
    } else

// XX Bummer.


#endif /* RESULT_H_ */
