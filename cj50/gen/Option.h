/*
  Copyright (C) 2021-2023 Christian Jaeger, <ch@christianjaeger.ch>
  Published under the terms of the MIT License, see the LICENSE file.
*/

//! An Option is a type that may sometimes represent some value, and
//! sometimes no value. Each Option type is specialized for the type of
//! the value that it may contain. For this reason, the type of that
//! contained value has to be specified as parameter to `Option`.
//! 
//! An Option has a field `is_some` which specifies whether the Option
//! value contains a value, in which case the field is `true`, or not (the
//! `none` case), and a field `value` which contains the actual value if
//! present (reading the `value` field while `is_some` is `false` is
//! undefined behaviour, don't do that!). It is the following struct, with
//! `T` replaced with the type passed to `Option(T)`:
//! 
//! ```C
//! typedef struct Option(T) {
//!     bool is_some;
//!     T value;
//! } Option(T);
//! ```
//! 
//! Member functions for the following generic functions are also defined:
//! 
//! `some(val)`
//! : Returns an Option where the `is_some` field is set to `true` and the `value` field is set to `val`.
//! 
//! `none(T)`
//! : Returns an Option where the `is_some` field is set to `false`. The `value` field is not valid. Also see `NONE` macro.
//! 
//! `drop(opt)`
//! : Calls `drop` on the value in the `value` field if `is_some` is `true`. CAUTION: only call drop when value has not been moved!
//! 
//! `equal(opt1, opt2)`
//! : Returns `true` if both arguments are structurally equivalent.
//! 
//! `unwrap(opt)`
//! : Returns the contents of the `value` field if `is_some` is `true`, otherwise aborts the program.
//! 
//! `print_debug(&opt)`
//! : Print a programmer's view of the Option value, given by reference.
//! 
//! `print_debug_move(opt)`
//! : Print a programmer's view of the Option value, given by copy.
//! 


#pragma once

#include <stdbool.h>
#include <cj50/basic-util.h>
#include <cj50/macro-util.h>


/// This macro creates a type name for an `Option` specific for the
/// given type name `T`.

/// Implementation wise, it simply concatenates `ref_` and the given
/// type name. For this reason, the type name `T` must not contain
/// spaces or `*`, e.g. `unsigned int` would not work and a typedef
/// like `uint` has to be used instead. For handling references, see
/// the `ref(T)` type generator.
#define Option(T) XCAT(Option_,T)

/// This macro defines the struct and functions to implement the type
/// `Option(T)`. It has to be used once for a given type
/// `T`. Afterwards `Option(T)` can be used any number of times.
#define GENERATE_Option(T)                                      \
    typedef struct Option(T) {                                  \
        bool is_some;                                           \
        T value;                                                \
    } Option(T);                                                \
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
    bool XCAT(equal_move_, Option(T))(Option(T) a,              \
                                      Option(T) b) {            \
        bool res = XCAT(equal_, Option(T))(&a, &b);             \
        XCAT(drop_, Option(T))(a);                              \
        XCAT(drop_, Option(T))(b);                              \
        return res;                                             \
    }                                                           \
                                                                \
    static UNUSED                                               \
    T XCAT(unwrap_, Option(T))(const Option(T) s) {             \
        if (s.is_some) {                                        \
            return s.value;                                     \
        } else {                                                \
            DIE("unwrap option: got none");                     \
        }                                                       \
    }                                                           \
                                                                \
    static UNUSED                                               \
    int XCAT(print_debug_, Option(T))(const Option(T) *s) {     \
        if (! s->is_some) {                                     \
            return printf("none(" STR(T) ")");                  \
        } else {                                                \
            int ret = 0;                                        \
            int res;                                            \
            res = printf("some(");                              \
            if (res < 0) { return res; }                        \
            ret += res;                                         \
            res = XCAT(print_debug_, T)(&s->value);             \
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
    int XCAT(print_debug_move_, Option(T))(const Option(T) s) { \
        return XCAT(print_debug_, Option(T))(&s);               \
    }


/// This macro is a short cut for `none(T)` but can only be used in
/// variable assignment, e.g.:

/// ```C
/// Option(float) x = NONE;
/// ```

/// It cannot be used in a function call like `f(NONE)`, instead
/// `f(none(float))` has to be used.

#define NONE { .is_some = false }


/// The constructor for some value of `T`. Example:

///```C
///     return Some(cstr)("foo")
///```

/// NOTE: only works if T is known textually
/// (e.g. `None(typeof(somevar))` does not work, while
/// `none(typeof(somevar))` does). 

#define Some(T) XCAT(some_, T)

/// The none value for `T`. Example:

///```C
///     return None(cstr);
///```

/// NOTE: only works if T is known textually
/// (e.g. `None(typeof(somevar))` does not work, while
/// `none(typeof(somevar))` does). The advantage over `none` is that
/// the latter is a generic that currently must be updated manually
/// and might be outdated or not in scope yet (within files of cj50
/// itself).

#define None(T) \
    XCAT(none_, T)()


/// This macro allows for convenient matching and conditional
/// execution depending on what case of the Option was
/// received. CAREFUL: it must always be paired with `else_None`, or
/// weird syntax errors will be reported because curly braces will not
/// be balanced! (The `else_None` always has to be there, but the `{
/// }` after it are optional.)

/// `var` is introduced in the given scope `{ .. }`. Note that you
/// don't need to specify a type for `var`, it is derived
/// automatically.

/// ```C
/// if_let_Some(w, get_float()) {
///     if_let_Some(h, get_float()) {
///         println(w * h);
///     } else_None {
///         goto cancelled;
///     }
/// } else_None {
///     goto cancelled;
/// }
/// ```

#define if_let_Some(var, expr)                  \
    AUTO HYGIENIC(res) = (expr);                \
    if (HYGIENIC(res).is_some) {                \
        AUTO var = HYGIENIC(res).value;

#define else_None                               \
    } else



/// Convenient conditional loop macro.

/// `var` is introduced in the given scope `{ .. }`. Note that you
/// don't need to specify a type for `var`, it is derived
/// automatically. You can't specify `UNUSED`, although you can pass
/// the variable to `drop` to silence the warning.

/// ```C
/// while_let_Some(v, pop(&vec)) {
///     DBG(v);
/// }
/// ```

#define while_let_Some(var, expr)                               \
    for (typeof((expr).value) var;                              \
         ({  AUTO HYGIENIC(res) = (expr);                       \
             var = HYGIENIC(res).value;                         \
             HYGIENIC(res).is_some; });                         \
        )


/// Set `var` to the value if `expr` evaluated to Some, otherwise run
/// the (statement or) block that is following.

/// The following example prints `x`.

/// ```C
/// let_Some_else(b1, some('x')) {
///     RETURN_Ok(None(u32));
/// }
/// print(b1);
/// ```

#define let_Some_else(var, expr)                \
    typeof((expr).value) var;                   \
    AUTO HYGIENIC(res) = (expr);                \
    if (HYGIENIC(res).is_some) {                \
        var = HYGIENIC(res).value;              \
    } else 

