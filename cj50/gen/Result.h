/*
  Copyright (C) 2021-2023 Christian Jaeger, <ch@christianjaeger.ch>
  Published under the terms of the MIT License, see the LICENSE file.
*/

//! ## Overview

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

//! ## Member functions

//! Member functions for the following generic functions are defined:

//! `equal(res1, res2)`
//! : Returns `true` if both arguments are structurally equivalent.

//! `unwrap(opt)`
//! : Returns the contents of the `ok` field if `is_ok` is `true`, otherwise aborts the program.

//! `print_debug(&opt)`
//! : Print a programmer's view of the Option value, given by reference.

//! `print_debug_move(opt)`
//! : Print a programmer's view of the Option value, given by copy.

//! ## Error propagation

//! By using normal if statements or `if_let_Ok`, you can build the
//! proper control flow to clean up and return from a function when
//! you receive an error via `Result` inside said function.
//! Alternatively, there is a set of macros that makes some of the
//! tedious parts more automatic: see `BEGIN_Result` and its related
//! macros.

//! Example:

//! ```C
//! Result(Unit, UnicodeError) run(char *str) {
//!     BEGIN_Result(Unit, UnicodeError);
//! 
//!     CFile in = TRY(memopen_CFile(str, strlen(str), "r"), cleanup1);
//! 
//!     Vec(ucodepoint) v = new_Vec_ucodepoint(); // get us an empty vector
//! 
//!     while_let_Some(c, TRY(get_ucodepoint_unlocked(&in), cleanup2)) {
//!         push(&v, c); // add c to the vector
//!     }
//!     DBG(v); // show the vector
//! 
//!     RETURN_Ok(Unit(), cleanup2); // we have nothing to return so use Unit()
//!     
//! cleanup2:
//!     drop(in);
//! cleanup1:
//!     END_Result();
//! }
//! ```

//! The function `run` here tries to get a `CFile` via
//! `memopen_CFile`, which can fail; if it failed, the error it
//! returned (a `SystemError`) is converted to a `UnicodeError`, the
//! program jumps to `cleanup1`, and returns that `UnicodeError` as the
//! Err variant of `Result(Unit, UnicodeError)`.

//! Otherwise, `in` is set to the successfully returned `CFile` value,
//! which needs to be `drop`ed later on.

//! `while_let_Some` from Option.h expects an Option as its second
//! argument. `get_ucodepoint_unlocked` returns
//! `Result(Option(ucodepoint), UnicodeError)`, though. so we use
//! `TRY` again to extract the `Option(ucodepoint)` in the successful
//! case, and jump to `cleanup2` in the error case. In that latter
//! case, the error is already of type `UnicodeError`, thus no
//! conversion is done, although it still calls the conversion
//! function `new_UnicodeError_from_UnicodeError` because there is no
//! way to handle special cases like this via macros.

//! After the cleanup actions (only one here), `END_Result()` actually
//! returns the success or error result that was stored in the context
//! set up by the `BEGIN_Result` macro call.

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
    void XCAT(drop_, Result(T, E))(Result(T, E) v) {            \
        if (v.is_ok) {                                          \
            XCAT(drop_, T)(v.ok);                               \
        } else {                                                \
            XCAT(drop_, E)(v.err);                              \
        }                                                       \
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
    bool XCAT(equal_move_, Result(T, E))(Result(T, E) a,        \
                                         Result(T, E) b) {      \
        bool res = XCAT(equal_, Result(T, E))(&a, &b);          \
        XCAT(drop_, Result(T, E))(a);                           \
        XCAT(drop_, Result(T, E))(b);                           \
        return res;                                             \
    }                                                           \
                                                                \
    static UNUSED                                               \
    T XCAT(unwrap_, Result(T, E))(const Result(T, E) s) {       \
        if (s.is_ok) {                                          \
            return s.ok;                                        \
        } else {                                                \
            XCAT(fprintln_, E)(stderr, &s.err);                 \
            XCAT(drop_, E)(s.err);                              \
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

#define Ok(T, E) XCAT(ok_, Result(T, E))

/// Returns the constructor function for the `err` variant of the `Result(T, E)` type for the given `T` and `E` types. Usage example:

/// ```C
/// Err(int, cstr)("the input does not contain a number")
/// ```

#define Err(T, E) XCAT(err_, Result(T, E))


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



/// This must be placed at the beginning of the function or block in
/// which you want to use `RETURN_Ok`, `RETURN_Err`, or
/// `TRY`.

#define BEGIN_Result(T, E)                      \
    Result(T, E) __propagate_return_val;

/// Requires a `BEGIN_Result` in its scope.

/// Stores `val` as an Ok Result in the context to be returned by
/// `END_Result` and jumps to `label`. This is to allow the program to
/// run cleanup actions.

#define RETURN_Ok(val, label)                   \
    __propagate_return_val.is_ok = true;        \
    __propagate_return_val.ok = (val);          \
    goto label;

/// Requires a `BEGIN_Result` in its scope.

/// Stores `errval` as an Err Result in the context to be returned by
/// `END_Result` and jumps to `label`. This is to allow the program to
/// run cleanup actions.

#define RETURN_Err(errval, label)               \
    __propagate_return_val.is_ok = false;       \
    __propagate_return_val.err = (errval);      \
    goto label;

/// Requires a `BEGIN_Result` in its scope.

/// `expr` must evaluate to a Result. If the Result is an Ok, the
/// unwrapped value is returned by the `TRY`
/// expression. If the Result is an Err, it is stored in the context
/// to be returned by `END_Result` and a jump to `label` is issued;
/// this is to enable the program to run cleanup actions.

/// `TRY` automatically inserts `new_from` calls to convert Err values
/// into the type that was defined using `BEGIN_Result`. If `expr`
/// returns a Result with an Err type `Given`, and
/// `BEGIN_Result(SomeType, Wanted)` was stated, this means that a
/// conversion function `new_Wanted_from_Given` needs to exist, which
/// needs to be part of the _Generic type dispatch in `new_from_` in
/// the file `cj50/gen/dispatch/new_from.h`.

#define TRY(expr, label)                             \
    ({                                               \
        typeof(expr) HYGIENIC(v) = (expr);           \
        if (! HYGIENIC(v).is_ok) {                   \
            __propagate_return_val.is_ok = false;    \
            __propagate_return_val.err = new_from(   \
                typeof(__propagate_return_val.err),  \
                HYGIENIC(v).err);                    \
            goto label;                              \
        }                                            \
        HYGIENIC(v).ok;                              \
    })


/// This must be placed at the very end of the function or block in
/// which you placed `BEGIN_Result`, after cleanup actions. It always
/// returns from the function with the Result that was temporarily
/// stored by `RETURN_Ok`, `RETURN_Err`, or `TRY`.

/// Requires a `BEGIN_Result` in its scope, as well as one of the 4
/// forms mentioned above (otherwise it returns an undefined value,
/// which the compiler will be warning about).

#define END_Result()                            \
    return __propagate_return_val;

