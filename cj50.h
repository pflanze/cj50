//! This is the main header file of the cj50 library collection.

//! It includes the various other files (see the [documentation
//! index](index.md)), and additionally provides the utilities
//! described below. Besides additional functions and macros, it
//! defines a number of generic functions that dispatch to functions
//! from various other files.

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>

#include "cj50/math.h"
#include "cj50/sdlutil.h"
#include "cj50/basic-util.h"
// #include "cj50/plot.h"  include it at the end to pass DBG
#include "cj50/string.h"
#include "cj50/String.h"
#include "cj50/char.h" /* already included via string.h though */
#include "cj50/int.h"
#include "cj50/uint.h"
#include "cj50/u64.h"
#include "cj50/gen/equal_array.h"
#include "cj50/gen/Result.h"
#include "cj50/float.h"
#include "cj50/double.h"
#include "cj50/random.h"
#include "cj50/bool.h"
#include "cj50/filecontents.h"
#include "cj50/xmem.h"


#define INIT_RESRET                             \
    __label__ cleanup; /* GCC extension */      \
    int ret = 0;                                \
    int res;

#define RESRET(e)                               \
    res = (e);                                  \
    if (res < 0) { ret = res; goto cleanup; }   \
    ret += res;


static
Option(string) _get_string() {
    while (true) {
#define SIZ 100
        char *line = malloc(SIZ);
        size_t len = SIZ;
#undef SIZ
        errno = 0;
        ssize_t n = getline(&line, &len, stdin);
        if (n < 0) {
            if (errno == 0) {
                free(line);
                return none_string();
            } else {
                DIE_("Could not get a line from stdin: %s",
                     strerror(errno));
            }
        }
        size_t l = strlen(line);
        // Always must have either a '\n' at the end or some other
        // text, since the end of file case is handled above.
        assert(l >= 1);
        if (line[l - 1] == '\n') {
            line[l - 1] = '\0';
            l--;
        }
        if (l > 0) {
            return some_string(line);
        }
        free(line);
        print_string("Your answer is empty. Please enter a string: ");
    }
}

/// Read a String from standard input, terminated by a
/// newline. Returns none on end of file (when ctl-d is pressed).

static UNUSED
Option(String) get_String() {
    Option(string) s = _get_string();
    if (s.is_some) {
        // need to discard const qualifier
        return some_String((String) { .str = (char*) s.value });
    } else {
        return none_String();
    }
}


static
int print_int(int n) {
    return printf("%i", n);
}


/// Values of this type (a number) describe the reason why a string
/// does not contain text that properly represents a value that the
/// used parse function should return.

/// Currently, numbers in the range 1..255 represent errors defined by
/// the operating system (errno), numbers in the range 500..29999 are
/// specific to cj50.h. But do *not* rely on any details of this type,
/// they may change, values of this type should just be passed to one
/// of the functions that accept it (`string_from_ParseError` and
/// `print_ParseError`).

typedef uint16_t ParseError;

static
bool equal_ParseError(const ParseError* a, const ParseError* b) {
    return *a == *b;
}

static
int print_debug_ParseError(const ParseError *v) {
    return print_int(*v);
}

const ParseError E_not_in_int_range = 500;
const ParseError E_invalid_text_after_number = 501;
const ParseError E_not_greater_than_zero = 502;
const ParseError E_negative = 503;
const ParseError E_not_a_number = 504;

// TODO: remove, should use new_Array_char instead, or just xmallocarray
static
char* new_string(size_t len);

/// Convert a `ParseError` value into a `string` for display. The
/// receiver owns the returned string.
static
char* string_from_ParseError(ParseError e) {
    if (e == E_not_in_int_range) {
        return xstrdup("is not within the range of numbers of the `int` type");
    } else if (e == E_invalid_text_after_number) {
        return xstrdup("has invalid text after the number");
    } else if (e == E_not_greater_than_zero) {
        return xstrdup("is not greater than zero");
    } else if (e == E_negative) {
        return xstrdup("is negative");
    } else if (e == E_not_a_number) {
        return xstrdup("is not a number");
    } else if (e < 256) {
#define SIZ_ 200
        char* s = new_string(SIZ_);
        assert(snprintf(s, SIZ_, "is not valid: %s", strerror(errno)) < SIZ_);
        return s;
#undef SIZ_
    } else {
        DIE("BUG: invalid ParseError value");
    }
}

static UNUSED
int print_ParseError(ParseError e) {
    INIT_RESRET;
    string s = NULL;
    RESRET(print_string("parse error: "));
    s = string_from_ParseError(e);
    RESRET(print_string(s));
cleanup:
    if (s) free((char*)s); // todo: don't hack, don't use `string`
    return ret;
}

static
int fprintln_ParseError(FILE* out, ParseError e) {
    INIT_RESRET;
    string s = string_from_ParseError(e);
    RESRET(fprintf(out, "parse error: input %s\n", s));
cleanup:
    if (s) free((char*)s); // todo: don't hack, don't use `string`
    return ret;
}

GENERATE_Result(int, ParseError);

/// Translate a string into an `int` if possible.
static
Result(int, ParseError) parse_int(string s) {
    // UBSan leads to strtol call causing exit(1), which prevents debugging
    assert(s);

    char *tail;
    errno = 0;
    long n = strtol(s, &tail, 10);
    if (errno == 0) {
        while (*tail == ' ') {
            tail++;
        }
        if (*tail == '\0') {
            if (n >= INT_MIN && n <= INT_MAX) {
                return Ok(int, ParseError)(n);
            } else {
                return Err(int, ParseError)(E_not_in_int_range);
            }
        } else {
            return Err(int, ParseError)(
                tail == s ? E_not_a_number :
                E_invalid_text_after_number);
        }
    } else {
        // return err_ParseResult(E_not_in_int_range);
        return Err(int, ParseError)(errno);
    }
}

#define GET_THING(T, type_desc, parse)                  \
    while (true) {                                      \
        Option(String) s = get_String();                \
        if (!s.is_some) {                               \
            return XCAT(none_, T)();                    \
        }                                               \
        Result(T, ParseError) r = parse(s.value.str);   \
        drop_Option_String(s);                          \
        if (r.is_ok) {                                  \
            return XCAT(some_, T)(r.ok);                \
        }                                               \
        print_string("Your answer ");                   \
        string errstr = string_from_ParseError(r.err);  \
        print_string(errstr);                           \
        free((char*)errstr); /* todo: don't hack, don't use `string` */ \
        print_string(". Please enter " type_desc ": "); \
    }

/// Read an integer number from standard input, terminated by a
/// newline. Returns none on end of file (when ctl-d is pressed).
static UNUSED
Option(int) get_int() {
    GET_THING(int, "an integer number", parse_int);
}


/// Translate a string into an `int` in the natural number range (1 or
/// higher) if possible.
static
Result(int, ParseError) parse_nat(string s) {
    AUTO rn = parse_int(s);
    if (rn.is_ok) {
        if (rn.ok > 0) {
            return rn; // ok
        } else {
            return Err(int, ParseError)(E_not_greater_than_zero);
        }
    } else {
        return rn; // err
    }
}


/// Read a natural number from standard input, terminated by a
/// newline. Returns none on end of file (when ctl-d is pressed).
static UNUSED
Option(int) get_nat() {
    GET_THING(int, "a natural number", parse_nat);
}

static UNUSED
int print_nat(int n) {
    if (n > 0) {
        return printf("%i", n);
    } else {
        DIE_("error: print_nat(%i): argument out of range", n);
    }
}


/// Translate a string into an `int` in the natural number range
/// including 0 (0 or higher) if possible.
static
Result(int, ParseError) parse_nat0(string s) {
    AUTO rn = parse_int(s);
    if (rn.is_ok) {
        if (rn.ok >= 0) {
            return rn; // ok
        } else {
            return Err(int, ParseError)(E_negative);
        }
    } else {
        return rn; // err
    }
}


/// Read a natural number or zero from standard input, terminated by a
/// newline. Returns none on end of file (when ctl-d is pressed).
static UNUSED
Option(int) get_nat0() {
    GET_THING(int, "a natural number or zero", parse_nat0);
}

static UNUSED
int print_nat0(int n) {
    if (n >= 0) {
        return printf("%i", n);
    } else {
        DIE_("error: print_nat0(%i): argument out of range", n);
    }
}


GENERATE_Result(float, ParseError);

/// Translate a string into an `float` if possible.
static
Result(float, ParseError) parse_float(string s) {
    char *tail;
    errno = 0;
    float x = strtof(s, &tail);
    if (errno == 0) {
        while (*tail == ' ') {
            tail++;
        }
        if (*tail == '\0') {
            return Ok(float, ParseError)(x);
        } else {
            return Err(float, ParseError)(
                tail == s ? E_not_a_number :
                E_invalid_text_after_number);
        }
    } else {
        return Err(float, ParseError)(errno);
    }
}

/// Read a floating point number or zero from standard input,
/// terminated by a newline. Returns none on end of file (when ctl-d is
/// pressed).
static UNUSED
Option(float) get_float() {
    GET_THING(float, "a floating point number", parse_float);
}


static
void* new_array_of_type_and_len(const char* typename,
                                size_t typesize,
                                size_t len) {
    void* p = calloc(len, typesize);
    if (p) {
        return p;
    } else {
        DIE_("Out of memory for an array of %ld %s elements",
             len, typename);
    }
}

// TODO: remove, should use new_Array_char instead, or just xmallocarray

/// Allocate and return a new array of `len` char values. Aborts
/// when there is not enough memory (never returns the NULL
/// pointer). The slots in the returned array are all initialized to
/// the '\0' char. This is the same as `new_string`.
static UNUSED
char* new_chars(size_t len) {
    return new_array_of_type_and_len("char", sizeof(char), len);
}

// TODO: remove, should use new_Array_char instead, or just xmallocarray

/// Allocate and return a new string of capacity `len`. Aborts when
/// there is not enough memory (never returns the NULL pointer). The
/// string is set to the empty string. This is the same as
/// `new_chars`.
static
char* new_string(size_t len) {
    return new_array_of_type_and_len("char", sizeof(char), len);
}

/// Allocate and return a new array of `len` string values. Aborts
/// when there is not enough memory (never returns the NULL
/// pointer). The slots in the returned array are all initialized to
/// the NULL pointer.
static UNUSED
string* new_strings(size_t len) {
    return new_array_of_type_and_len("string", sizeof(string), len);
}


/// Allocate and return a new array of `len` Option(String)
/// values. Aborts when there is not enough memory (never returns the
/// NULL pointer). The slots in the returned array are all initialized
/// to the none value.
static UNUSED
Option(String)* new_Option_Strings(size_t len) {
    return new_array_of_type_and_len("Option(String)", sizeof(Option(String)), len);
}


static
void drop_strings_slice(string* ary, size_t len) {
    for (size_t i = 0; i < len; i++) {
        string s = ary[i];
        if (s) {
            free((char*)s); /* todo: don't hack, don't use `string` */
        }
    }
}

static UNUSED
void drop_strings(string* ary, size_t len) {
    drop_strings_slice(ary, len); /* todo: stop doing that, borrowed */
    free(ary);
}

static
void drop_Option_Strings_slice(Option(String)* ary, size_t len) {
    for (size_t i = 0; i < len; i++) {
        drop_Option_String(ary[i]);
    }
}

static UNUSED
void drop_Option_Strings(Option(String)* ary, size_t len) {
    drop_Option_Strings_slice(ary, len);
    free(ary);
}


/// Allocate and return a new array of `len` int values. Aborts when
/// there is not enough memory (never returns the NULL pointer). The
/// slots in the returned array are all initialized to the value 0.
static UNUSED
int* new_ints(size_t len) {
    return new_array_of_type_and_len("int", sizeof(int), len);
}

/// Allocate and return a new array of `len` float values. Aborts when
/// there is not enough memory (never returns the NULL pointer). The
/// slots in the returned array are all initialized to the value 0.
static UNUSED
float* new_floats(size_t len) {
    return new_array_of_type_and_len("float", sizeof(float), len);
}

/*
  does this work?
  #define INITIALIZE_ARRAY(ary, members) *ary = members
  */

#define PRINT_ARRAY(print_typ, ary, len)        \
    INIT_RESRET;                                \
    RESRET(print_string("{"));                  \
    for (size_t i = 0; i < len; i++) {          \
        if (i > 0) {                            \
            RESRET(print_string(", "));         \
        }                                       \
        RESRET(print_typ(&ary[i]));             \
    }                                           \
    RESRET(print_string("}"));                  \
cleanup:                                        \
    return ret;

static UNUSED
int print_debug_chars(const char* ary, size_t len) {
    PRINT_ARRAY(print_debug_char, ary, len);
}

static UNUSED
int print_debug_strings(const string* ary, size_t len) {
    PRINT_ARRAY(print_debug_string, ary, len);
}

static UNUSED
int print_debug_Option_Strings(const Option(String)* ary, size_t len) {
    PRINT_ARRAY(print_debug_Option_String, ary, len);
}

static UNUSED
int print_debug_ints(const int* ary, size_t len) {
    PRINT_ARRAY(print_debug_int, ary, len);
}

static UNUSED
int print_debug_floats(const float* ary, size_t len) {
    PRINT_ARRAY(print_debug_float, ary, len);
}

#undef PRINT_ARRAY


/// Prints the given value for normal text use.

/// Returns an `int` that when negative signifies an error (check
/// `errno` for the error in this case), and when positive signifies
/// the number of bytes written.

/// Also see `print_debug` and `print_debug_array`, which support more
/// types, but are not meant for printing output for normal humans.

#define print(v)                                \
    _Generic((v)                                \
             , String: print_String             \
             , const char*: print_string        \
             , char*: print_string              \
             , char: putchar                    \
             , int: print_int                   \
             , uint: print_uint                 \
             , u64: print_u64                   \
             , float: print_float               \
             , double: print_double             \
        )(v)

#define GENERATE_PRINTLN(T)                     \
    static UNUSED                               \
    int XCAT(println_, T)(T v) {                \
        INIT_RESRET;                            \
        RESRET(print(v));                       \
        RESRET(print("\n"));                    \
    cleanup:                                    \
        return ret;                             \
    }


GENERATE_PRINTLN(string);
GENERATE_PRINTLN(String);
GENERATE_PRINTLN(char);
GENERATE_PRINTLN(int);
GENERATE_PRINTLN(uint);
GENERATE_PRINTLN(u64);
GENERATE_PRINTLN(float);
GENERATE_PRINTLN(double);

/// Prints the given value for normal text use, followed by a newline
/// ("\n") for convenience.

#define println(v)                              \
    _Generic((v)                                \
             , String: println_String           \
             , char*: println_string            \
             , char: println_char               \
             , int: println_int                 \
             , uint: println_uint               \
             , u64: println_u64                 \
             , float: println_float             \
             , double: println_double           \
        )(v)

/// Prints the given value in a programmer's view, for debugging
/// purposes.

/// Returns an `int` that when negative signifies an error (check
/// `errno` for the error in this case), and when positive signifies
/// the number of bytes written.

/// See `print` for printing as normal text, not for debugging, and
/// `print_debug_array` for printing arrays.

#define print_debug(v)                                        \
    _Generic((v)                                              \
             , String*: print_debug_String                    \
             , String: print_debug_move_String                \
             , char*: print_debug_string                      \
             , bool: print_debug_bool                         \
             , char: print_debug_char                         \
             , int: print_int                                 \
             , uint: print_uint                               \
             , u64: print_u64                                 \
             , float: print_float                             \
             , double: print_double                           \
             , Vec2: print_debug_Vec2                         \
             , Vec3: print_debug_Vec3                         \
             , Option(string): print_debug_move_Option_string \
             , Option(String): print_debug_move_Option_String \
             , Option(int): print_debug_move_Option_int       \
             , Option(float): print_debug_move_Option_float   \
             , Option(string)*: print_debug_Option_string     \
             , Option(String)*: print_debug_Option_String     \
             , Option(int)*: print_debug_Option_int           \
             , Option(float)*: print_debug_Option_float       \
             , Line2: print_debug_Line2                       \
             , Rect2: print_debug_Rect2                       \
             , Result(int, ParseError)*: print_debug_Result_int__ParseError \
             , Result(int, ParseError): print_debug_move_Result_int__ParseError \
             , Result(String, SystemError): print_debug_move_Result_String__SystemError \
             , SystemError: print_debug_SystemError                     \
        )(v)


/// Prints the given array in a programmer's view, for debugging
/// purposes.

/// Returns an `int` that when negative signifies an error (check
/// `errno` for the error in this case), and when positive signifies
/// the number of bytes written.

/// See `print_debug` for printing non-array values.

#define print_debug_array(v, len)                               \
    _Generic((v)                                                \
             , char*: print_debug_chars                         \
             , string*: print_debug_strings                     \
             , Option(String)*: print_debug_Option_Strings      \
             , int*: print_debug_ints                           \
             , float*: print_debug_floats                       \
        )((v), (len))



/// Frees the resources held by the given value (including resources
/// held by contained values, recursively, unlike the `free` function).
#define drop(v)                                        \
    _Generic((v)                                       \
             , String: drop_String                     \
             , const char*: drop_string                \
             , char*: drop_string                      \
             , int*: free                              \
             , float*: free                            \
             , Vec2*: free                             \
             , Vec3*: free                             \
             , Option(string): drop_Option_string      \
             , Option(String): drop_Option_String      \
             , Option(int): drop_Option_int            \
             , Option(float): drop_Option_float        \
             , Option(double): drop_Option_double      \
        )(v)



#define RESIZE_ARRAY_test_equal(ary, oldlen, newlen)    \
    if (newlen == oldlen) {                             \
        return ary;                                     \
    }
#define RESIZE_ARRAY_free(ary, oldlen, newlen)          \
    if (newlen < oldlen) {                              \
        for (size_t i = newlen; i < oldlen; i++) {      \
            if (ary[i]) { free((void*)ary[i]); } /* todo don't hack, don't use `string` */ \
        }                                               \
    }
#define RESIZE_ARRAY_drop(ary, oldlen, newlen)          \
    if (newlen < oldlen) {                              \
        for (size_t i = newlen; i < oldlen; i++) {      \
            drop(ary[i]);                               \
        }                                               \
    }
#define RESIZE_ARRAY_realloc_and_fill(ary, oldlen, newlen, T, null)     \
    T* res = reallocarray(ary, newlen, sizeof(T));                      \
    if (res) {                                                          \
        if (newlen > oldlen) {                                          \
            for (size_t i = oldlen; i < newlen; i++) {                  \
                res[i] = null;                                          \
            }                                                           \
        }                                                               \
        return res;                                                     \
    } else {                                                            \
        DIE_("Out of memory to resize array from %ld to %ld %s members",\
             oldlen, newlen, STR(T));                                   \
    }


static UNUSED
string* resize_strings(string* ary, size_t oldlen, size_t newlen) {
    RESIZE_ARRAY_test_equal(ary, oldlen, newlen);
    RESIZE_ARRAY_free(ary, oldlen, newlen);
    RESIZE_ARRAY_realloc_and_fill(ary, oldlen, newlen, string, NULL);
}

static UNUSED
Option(String)* resize_Option_Strings(
    Option(String)* ary, size_t oldlen, size_t newlen
    ) {
    RESIZE_ARRAY_test_equal(ary, oldlen, newlen);
    RESIZE_ARRAY_drop(ary, oldlen, newlen);
    RESIZE_ARRAY_realloc_and_fill(
        ary, oldlen, newlen, Option(String), none_String());
}

static UNUSED
int* resize_ints(int* ary, size_t oldlen, size_t newlen) {
    RESIZE_ARRAY_test_equal(ary, oldlen, newlen);
    RESIZE_ARRAY_realloc_and_fill(ary, oldlen, newlen, int, 0);
}

static UNUSED
float* resize_floats(float* ary, size_t oldlen, size_t newlen) {
    RESIZE_ARRAY_test_equal(ary, oldlen, newlen);
    RESIZE_ARRAY_realloc_and_fill(ary, oldlen, newlen, float, 0);
}


#undef RESIZE_ARRAY_test_equal
#undef RESIZE_ARRAY_free
#undef RESIZE_ARRAY_realloc_and_fill

/// Expects an array as the first argument, and the current length of
/// the array and the desired new length. Returns a new array of the
/// desired new length. If the new length is shorter than the old
/// length, the slots that are freed are `drop`ped, if the new length
/// is larger, then the new slots are filled in with blanks (zero
/// values, NULL pointers, or similar).
#define resize(v, oldlen, newlen)                       \
    _Generic((v)                                        \
             , string*: resize_strings                  \
             , Option(String)*: resize_Option_Strings   \
             , int*: resize_ints                        \
             , float*: resize_floats                    \
        )((v), (oldlen), (newlen))

/// Frees the resources held by the given array,
/// including all the values contained in the array (including
/// resources held by contained values, recursively). For arrays
/// holding pointers, array slots containing the NULL pointer are
/// skipped.
#define drop_array(v, len)                              \
    _Generic((v)                                        \
             , string*: drop_strings                    \
             , Option(String)*: drop_Option_Strings     \
        )((v), (len))


/// Takes two values of the same type and returns a `bool`. Returns
/// `true` if `a` and `b` are structurally equivalent.
#define equal(a, b)                                                     \
    _Generic((a)                                                        \
             , Option(string)*: equal_Option_string                     \
             , Option(String)*: equal_Option_String                     \
             , Option(int)*: equal_Option_int                           \
             , Option(float)*: equal_Option_float                       \
             , Result(int, ParseError)*: equal_Result_int__ParseError   \
             , Result(String, SystemError)*: equal_Result_String__SystemError \
             , string*: equal_string                                    \
             , String*: equal_String                                    \
             , string: equal_move_string                                \
             , int*: equal_int                                          \
             , int: equal_move_int                                      \
             , float*: equal_float                                      \
             , float: equal_move_float                                  \
             , double*: equal_double                                    \
             , double: equal_move_double                                \
             , bool*: equal_bool                                        \
             , bool: equal_move_bool                                    \
             , char: equal_move_char                                    \
        )((a), (b))

/// Takes a value of some type `T` and returns a `some` variant of
/// `Option(T)` containing the value.
#define some(v)                                         \
    _Generic((v)                                        \
             , string: some_string                      \
             , String: some_String                      \
             , int: some_int                            \
             , u64: some_u64                            \
             , float: some_float                        \
             , double: some_double                      \
        )(v)

/// Returns the none variant of `Option(T)` for the given
/// T. Equivalent to `none_T()` using the type name of T, but
/// `none(T)` allows T to also be a `typeof` expression.
#define none(T)                                         \
    _Generic(*((T*)(NULL))                              \
             , string: none_string                      \
             , String: none_String                      \
             , int: none_int                            \
             , u64: none_u64                            \
             , float: none_float                        \
             , double: none_double                      \
        )()

/// Takes a wrapper for some type `T` (like `Option(T)`, or `Result(T,
/// E)`) and returns the contained value of type `T` if possible, or
/// aborts if not possible.

/// Example:
///
/// ```C
/// Option(string) maybe_name = get_string();
/// // maybe_name can be a `none(string)` or e.g. `some("Alex")`.
/// string name = unwrap(maybe_name);
/// // Now we're guaranteed to have a string in `name`, "Alex" if we
/// // had the second case above. But if `maybe_name` was a none
/// // then the program has terminated by now instead.
/// ```

#define unwrap(v)                                                       \
    _Generic((v)                                                        \
             , Option(string): unwrap_Option_string                     \
             , Option(String): unwrap_Option_String                     \
             , Option(int): unwrap_Option_int                           \
             , Option(u64): unwrap_Option_u64                           \
             , Option(float): unwrap_Option_float                       \
             , Option(double): unwrap_Option_double                     \
             , Result(int, ParseError): unwrap_Result_int__ParseError   \
             , Result(float, ParseError): unwrap_Result_float__ParseError   \
             , Result(String, SystemError): unwrap_Result_String__SystemError \
        )(v)

/// Returns the name of the type of `e` as a string constant.
#define type_name(e)                                    \
    _Generic((e)                                        \
             , string: "string"                         \
             , String: "String"                         \
             , int: "int"                               \
             , unsigned int: "uint"                     \
             , u64: "u64"                               \
             , float: "float"                           \
             , double: "double"                         \
             , Option(string): "Option(string)"         \
             , Option(String): "Option(String)"         \
             , Option(int): "Option(int)"               \
             , Option(u64): "Option(u64)"               \
             , Option(float): "Option(float)"           \
             , Option(double): "Option(double)"         \
             , Vec2: "Vec2"                             \
             , Vec3: "Vec3"                             \
             , Rect2: "Rect2"                           \
             , Line2: "Line2"                           \
             , Result(int, ParseError): "Result(int, ParseError)"       \
             , Result(String, SystemError): "Result(String, SystemError)" \
        )


GENERATE_equal_array(char);
GENERATE_equal_array(string);
GENERATE_equal_array(String);
GENERATE_equal_array(int);
GENERATE_equal_array(uint);
GENERATE_equal_array(u64);
GENERATE_equal_array(float);
GENERATE_equal_array(double);

/// Returns true iff len1 == len2 and for every index,
/// `equal(array1[i], array2[i])` is true.
#define equal_array(array1, len1, array2, len2)         \
    _Generic((array1)[0]                                \
             , char: equal_array_char                   \
             , string: equal_array_string               \
             , String: equal_array_String               \
             , int: equal_array_int                     \
             , uint: equal_array_uint                   \
             , u64: equal_array_u64                     \
             , float: equal_array_float                 \
             , double: equal_array_double               \
        )((array1), (len1), (array2), (len2))


// These are documented in Result.h -- removed now
// #define ok(val, E) ...
// #define err(val, E) ...


/// "DeBuG": print the expression `expr` and the value it evaluated
/// to, for debugging purposes (calls `print_debug` on the value).
#define DBG(expr)                               \
    do {                                        \
        print("DEBUG: " #expr " == ");          \
        print_debug(expr);                      \
        print("\n");                            \
    } while (0)


static
bool is_alpha_underscore(char c) {
    return isalpha(c) || c == '_';
}

static
bool is_symbol(const char* s) {
    size_t len = strlen(s);
    if ((len > 0) && is_alpha_underscore(s[0])) {
        for (size_t i = 1; i < len; i++) {
            char c = s[i];
            if (! (is_alpha_underscore(c) || isdigit(c))) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

static UNUSED
int print_var_or_expr(const char* s) {
    INIT_RESRET;
    if (is_symbol(s)) {
        RESRET(print_string(s));
    } else {
        RESRET(print_string("("));
        RESRET(print_string(s));
        RESRET(print_string(")"));
    }
cleanup:
    return ret;
}

/// `DBG` `A`rray: print the expression `expr` and the array value
/// it evaluated to, for debugging purposes (calls
/// `print_debug_array`). `len` must give the length of the array.
#define DBGA(expr, len)                                 \
    do {                                                \
        print("DEBUG: ");                               \
        print(type_name((expr)[0]));                    \
        print(" ");                                     \
        print_var_or_expr(#expr);                       \
        print("[" #len "] == ");                        \
        print_debug_array(expr, len);                   \
        print("\n");                                    \
    } while (0)


#undef GET_THING
#undef RESRET
#undef INIT_RESRET

#include "cj50/plot.h"
