/*
  Copyright (C) 2021-2023 Christian Jaeger, <ch@christianjaeger.ch>
  Published under the terms of the MIT License, see the LICENSE file.
*/

//! A Result is a value that has two variants and can represent either
//! success with a value of one type, `T`, or an error with an error
//! value of another type, `E`. It has a field `is_ok` that is `true`
//! if the Result represents success, in which case the field `ok` is
//! valid, otherwise the field `err` is valid.

//! ```C
//! typedef struct Result(T, E) {
//!     bool is_ok;
//!     union {
//!         T ok;
//!         E err;
//!     };
//! } Result(T, E);
//! ```

 //! Member functions for the following generic functions are also defined:

//! `equal(res1, res2)`
//! : Returns `true` if both arguments are structurally equivalent.

//! `unwrap(opt)`
//! : Returns the contents of the `ok` field if `is_ok` is `true`, otherwise aborts the program.

//! `print_debug(&opt)`
//! : Print a programmer's view of the Option value, given by reference.

//! `print_debug_move(opt)`
//! : Print a programmer's view of the Option value, given by copy.

/* future?:

! `ok(val, E)`
! : Returns a Result where the `is_ok` field is set to `true` and the `ok` field is set to `val`.

! `err(T, e)`
! : Returns a Result where the `is_ok` field is set to `false` and the `err` field is set to `e`.

*/


#pragma once

#include <cj50/macro-util.h>


/// This macro creates a type name for a `Result` specific for the
/// given type names `T` and `E`.

/// Implementation wise, it simply concatenates `Result_`, `T`, `__`,
/// `E`. For this reason, the type names `T` and `E` must not contain
/// spaces, e.g. `unsigned int` would not work and a typedef like
/// `uint` has to be used instead.
#define Result(T, E) XCAT(Result_, XCAT(T, XCAT(__, E)))

#define GENERATE_Result(T, E)                                   \
    typedef struct Result(T, E) {                               \
        bool is_ok;                                             \
        union {                                                 \
            T ok;                                               \
            E err;                                              \
        };                                                      \
    } Result(T, E);                                             \
                                                                \
    static UNUSED                                               \
    Result(T, E) XCAT(ok_, Result(T, E))(T v) {                 \
        return (Result(T, E)) { .is_ok = true, .ok = v };       \
    }                                                           \
                                                                \
    static UNUSED                                               \
    Result(T, E) XCAT(err_, Result(T, E))(E e) {                \
        return (Result(T, E)) { .is_ok = false, .err = e };     \
    }                                                           \
                                                                \
    static UNUSED                                               \
    bool XCAT(equal_, Result(T, E))(const Result(T, E) *a,      \
                                    const Result(T, E) *b) {    \
        return ((a->is_ok == b->is_ok) &&                       \
                (a->is_ok                                       \
                 ? XCAT(equal_, T)(&a->ok, &b->ok)              \
                 : XCAT(equal_, E)(&a->err, &b->err)));         \
    }                                                           \
                                                                \
    static UNUSED                                               \
    T XCAT(unwrap_, Result(T, E))(const Result(T, E) s) {       \
        if (s.is_ok) {                                          \
            return s.ok;                                        \
        } else {                                                \
            XCAT(fprintln_, E)(stderr, s.err);                  \
            fflush(stderr);                                     \
            abort();                                            \
        }                                                       \
    }                                                           \
                                                                \
    static UNUSED                                               \
    int XCAT(print_debug_, Result(T, E))(                       \
        const Result(T, E) *s) {                                \
        int ret = 0;                                            \
        int res;                                                \
        const char* constr = s->is_ok ? "Ok" : "Err";           \
        res = printf("%s(%s, %s)(", constr, #T, #E);            \
        if (res < 0) { return res; } ret += res;                \
        if (s->is_ok) {                                         \
            res = XCAT(print_debug_, T)(&s->ok);                \
        } else {                                                \
            res = XCAT(print_debug_, E)(&s->err);               \
        }                                                       \
        if (res < 0) { return res; } ret += res;                \
        res = printf(")");                                      \
        if (res < 0) { return res; } ret += res;                \
        return ret;                                             \
    }                                                           \
                                                                \
    /* This is a HACK to allow print_debug */                   \
    /* to accept arguments without & */                         \
    static UNUSED                                               \
    int XCAT(print_debug_move_, Result(T, E))(                  \
        const Result(T, E) s) {                                 \
        return XCAT(print_debug_, Result(T, E))(&s);            \
    }                                                           \



/// Returns the constructor function for the `ok` variant of the `Result(T, E)` type for the given `T` and `E` types. Usage example:

/// ```C
/// Ok(int, cstr)(42)
/// ```
#define Ok(T, E)                                \
    XCAT(ok_, Result(T, E))

/// Returns the constructor function for the `err` variant of the `Result(T, E)` type for the given `T` and `E` types. Usage example:

/// ```C
/// Err(int, cstr)("the input does not contain a number")
/// ```
#define Err(T, E)                               \
    XCAT(err_, Result(T, E))


/// This macro allows for convenient matching and conditional
/// execution depending on what case of the Result was
/// received. CAREFUL: it must always be paired with `else_Err` and
/// `end_let_Ok`, or weird syntax errors will be reported because
/// curly braces will not be balanced!

/// The `var`s are introduced in the given scopes `{ .. }`. Note that
/// you don't need to specify a type for them, they are derived
/// automatically. You can still give modifiers like `UNUSED` or
/// `const`.

/// ```C
/// if_let_Ok(cnt, filecontents_String(path)) {
///     print(cnt);
///     drop(cnt);
/// } else_Err(e) {
///     fprintln_SystemError(stderr, e);
/// } end_let_Ok;
/// ```

#define if_let_Ok(var, expr)                    \
    {                                           \
    AUTO ___if_let_res = (expr);                \
    if (___if_let_res.is_ok) {                  \
        AUTO var = ___if_let_res.ok;

#define else_Err(var)                           \
    } else {                                    \
        AUTO var = ___if_let_res.err;

#define end_let_Ok                              \
    }}

