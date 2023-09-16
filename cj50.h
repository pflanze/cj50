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

#include "cj50/unicode.h"
#include "cj50/math.h"
#include "cj50/sdlutil.h"
#include "cj50/basic-util.h"
// #include "cj50/plot.h"  include it at the end to pass DBG
#include "cj50/gen/Vec.h"
#include "cj50/CStr.h"
#include "cj50/String.h"
#include "cj50/char.h" /* already included via CStr.h though */
#include "cj50/int.h"
#include "cj50/uint.h"
#include "cj50/u8.h"
#include "cj50/u64.h"
#include "cj50/i64.h"
#include "cj50/gen/equal_array.h"
#include "cj50/gen/Result.h"
#include "cj50/float.h"
#include "cj50/double.h"
#include "cj50/random.h"
#include "cj50/bool.h"
#include "cj50/filecontents.h"
#include "cj50/xmem.h"
#include "cj50/parseError.h"
#include "cj50/resret.h"
#include "cj50/instantiations/Vec_uppercase_CStr.h"
#include "cj50/instantiations/Vec_cstr.h"
#include "cj50/instantiations/Vec_ucodepoint.h"
#include "cj50/instantiations/Vec_utf8char.h"
#include <cj50/instantiations/Vec_char.h>
#include <cj50/instantiations/Result_Unit__String.h>
#include <cj50/os_thread.h>
#include <cj50/gen/Mutex.h>
#include <cj50/instantiations/Vec_int.h>
#include <cj50/instantiations/Vec_Vec2_int.h>
#include <cj50/instantiations/Vec_Vec2_float.h>
#include <cj50/instantiations/Vec_Vec2_double.h>
#include <cj50/instantiations/Vec_double.h>
#include <cj50/instantiations/Vec_float.h>
#include <cj50/instantiations/Vec2_u32.h>
#include <cj50/instantiations/Vec3_u32.h>


// Read a CStr from `inp`, terminated by a newline (the newline is
// not included in the returned string). Returns none on end of file
// (or in the case of terminal input, when ctl-d is pressed). In case
// of errors, it dies with a message that shows the `filename`.

// -- do not document, does not make sense, would have to take a
// -- filehandle to a tty and print to that, too! Also, has no max_len
// -- protection.

static
Option(CStr) fget_CStr(FILE *inp, cstr filename) {
    while (true) {
#define SIZ 100
        char *line = malloc(SIZ);
        size_t len = SIZ;
#undef SIZ
        errno = 0;
        ssize_t n = getline(&line, &len, inp);
        if (n < 0) {
            if (errno == 0) {
                free(line);
                return none_CStr();
            } else {
                DIE_("Could not get a line from '%s': %s",
                     filename,
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
            return some_CStr(CStr_from_cstr_unsafe(line));
        }
        free(line);
        print_move_cstr("Your answer is empty. Please enter a string: ");
    }
}


/// Read a CStr from stdin (standard input), terminated by a newline
/// (the newline is not included in the returned string). Returns none
/// on end of file (when ctl-d is pressed).

static
Option(CStr) get_CStr() {
    return fget_CStr(stdin, "stdin");
}

/// Read a String from standard input, terminated by a
/// newline. Returns none on end of file (when ctl-d is pressed).

/// Note: currently can't take a `'\0'` as it works via `get_CStr`.

static UNUSED
Option(String) get_String() {
    Option(CStr) s = get_CStr();
    if (s.is_some) {
        return some_String(new_String_from_CStr(s.value));
    } else {
        return none_String();
    }
}


/// Translate a string into an `int` if possible.
static
Result(int, ParseError) parse_int(cstr s) {
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
                return Err(int, ParseError)(ParseError(E_not_in_int_range));
            }
        } else {
            return Err(int, ParseError)(ParseError(
                                            tail == s ? E_not_a_number :
                                            E_invalid_text_after_number));
        }
    } else {
        // return err_ParseResult(E_not_in_int_range);
        return Err(int, ParseError)(ParseError(errno));
    }
}

#define GET_THING(T, type_desc, parse)                          \
    while (true) {                                              \
        Option(CStr) s = get_CStr();                            \
        if (!s.is_some) {                                       \
            return XCAT(none_, T)();                            \
        }                                                       \
        Result(T, ParseError) r = parse(s.value.cstr);          \
        drop_Option_CStr(s);                                    \
        if (r.is_ok) {                                          \
            return XCAT(some_, T)(r.ok);                        \
        }                                                       \
        print_move_cstr("Your answer ");                        \
        CStr errstr = string_from_ParseError(&r.err);           \
        print_CStr(&errstr);                                    \
        drop_CStr(errstr);                                      \
        /* drop_ParseError(r.err); */                           \
        print_move_cstr(". Please enter " type_desc ": ");      \
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
Result(int, ParseError) parse_nat(cstr s) {
    AUTO rn = parse_int(s);
    if (rn.is_ok) {
        if (rn.ok > 0) {
            return rn; // ok
        } else {
            return Err(int, ParseError)(ParseError(E_not_greater_than_zero));
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
Result(int, ParseError) parse_nat0(cstr s) {
    AUTO rn = parse_int(s);
    if (rn.is_ok) {
        if (rn.ok >= 0) {
            return rn; // ok
        } else {
            return Err(int, ParseError)(ParseError(E_negative));
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

/// Translate a cstr into an `float` if possible.
static
Result(float, ParseError) parse_float(cstr s) {
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
            return Err(float, ParseError)(ParseError(
                                              tail == s ? E_not_a_number :
                                              E_invalid_text_after_number));
        }
    } else {
        return Err(float, ParseError)(ParseError(errno));
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
/// the '\0' char.
static UNUSED
char* new_chars(size_t len) {
    return new_array_of_type_and_len("char", sizeof(char), len);
}


/// Allocate and return a new array of `len` cstr values. Aborts
/// when there is not enough memory (never returns the NULL
/// pointer). The slots in the returned array are all initialized to
/// the NULL pointer.
static UNUSED
cstr* new_cstrs(size_t len) {
    return new_array_of_type_and_len("cstr", sizeof(cstr), len);
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
void drop_strings_slice(cstr* ary, size_t len) {
    for (size_t i = 0; i < len; i++) {
        cstr s = ary[i];
        if (s) {
            free((char*)s); /* todo: don't hack, don't use `cstr` */
        }
    }
}

static UNUSED
void drop_cstrs(cstr* ary, size_t len) {
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
    RESRET(print_move_cstr("{"));               \
    for (size_t i = 0; i < len; i++) {          \
        if (i > 0) {                            \
            RESRET(print_move_cstr(", "));      \
        }                                       \
        RESRET(print_typ(&ary[i]));             \
    }                                           \
    RESRET(print_move_cstr("}"));               \
cleanup:                                        \
    return ret;

static UNUSED
int print_debug_chars(const char* ary, size_t len) {
    PRINT_ARRAY(print_debug_char, ary, len);
}

static UNUSED
int print_debug_cstrs(const cstr* ary, size_t len) {
    PRINT_ARRAY(print_debug_cstr, ary, len);
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

#define print(v)                                        \
    _Generic((v)                                        \
             , char: putchar                            \
             , int*: print_int                          \
             , const int*: print_int                    \
             , int: print_move_int                      \
             , u8*: print_u8                            \
             , const u8*: print_u8                      \
             , u8: print_move_u8                        \
             , u32*: print_u32                          \
             , const u32*: print_u32                    \
             , u32: print_move_u32                      \
             , u64*: print_u64                          \
             , const u64*: print_u64                    \
             , u64: print_move_u64                      \
             , i64*: print_i64                          \
             , const i64*: print_i64                    \
             , i64: print_move_i64                      \
             , float*: print_float                      \
             , const float*: print_float                \
             , float: print_move_float                  \
             , double*: print_double                    \
             , const double*: print_double              \
             , double: print_move_double                \
             , char*: print_move_cstr                   \
             , cstr*: print_cstr                        \
             , const cstr*: print_cstr                  \
             , cstr: print_move_cstr                    \
             , ucodepoint*: print_ucodepoint            \
             , const ucodepoint*: print_ucodepoint      \
             , ucodepoint: print_move_ucodepoint        \
             , CStr*: print_CStr                        \
             , const CStr*: print_CStr                  \
             , CStr: print_move_CStr                    \
             , String*: print_String                    \
             , const String*: print_String              \
             , String: print_move_String                \
             , utf8char*: print_utf8char                \
             , const utf8char*: print_utf8char          \
             , utf8char: print_move_utf8char            \
        )(v)
// ^ uint is covered by either u32 or u64, depending on architecture.


#define GENERATE_PRINTLN(T)                     \
    static UNUSED                               \
    int XCAT(println_, T)(const T *v) {         \
        INIT_RESRET;                            \
        RESRET(print(v));                       \
        RESRET(print_move_cstr("\n"));          \
    cleanup:                                    \
        return ret;                             \
    }                                           \
    static UNUSED                               \
    int XCAT(println_move_, T)(T v) {           \
        INIT_RESRET;                            \
        RESRET(print(v));                       \
        RESRET(print_move_cstr("\n"));          \
    cleanup:                                    \
        return ret;                             \
    }
GENERATE_PRINTLN(cstr);
GENERATE_PRINTLN(char);
GENERATE_PRINTLN(int);
GENERATE_PRINTLN(uint);
GENERATE_PRINTLN(u8);
GENERATE_PRINTLN(u32);
GENERATE_PRINTLN(u64);
GENERATE_PRINTLN(i64);
GENERATE_PRINTLN(float);
GENERATE_PRINTLN(double);
GENERATE_PRINTLN(String);
GENERATE_PRINTLN(CStr);
GENERATE_PRINTLN(utf8char);
GENERATE_PRINTLN(ucodepoint);



/// Prints the given value for normal text use, followed by a newline
/// (`"\n"`) for convenience.

#define println(v)                                        \
    _Generic((v)                                          \
             , char: println_move_char                    \
             , int: println_int                           \
             , u8*: println_u8                            \
             , const u8*: println_u8                      \
             , u8: println_move_u8                        \
             , u32*: println_u32                          \
             , const u32*: println_u32                    \
             , u32: println_move_u32                      \
             , u64*: println_u64                          \
             , const u64*: println_u64                    \
             , u64: println_move_u64                      \
             , i64*: println_i64                          \
             , const i64*: println_i64                    \
             , i64: println_move_i64                      \
             , float*: println_float                      \
             , const float*: println_float                \
             , float: println_move_float                  \
             , double*: println_double                    \
             , const double*: println_double              \
             , double: println_move_double                \
             , char*: println_move_cstr                   \
             , cstr*: println_cstr                        \
             , const cstr*: println_cstr                  \
             , cstr: println_move_cstr                    \
             , ucodepoint*: println_ucodepoint            \
             , const ucodepoint*: println_ucodepoint      \
             , ucodepoint: println_move_ucodepoint        \
             , CStr*: println_CStr                        \
             , const CStr*: println_CStr                  \
             , CStr: println_move_CStr                    \
             , String*: println_String                    \
             , const String*: println_String              \
             , String: println_move_String                \
             , utf8char*: println_utf8char                \
             , const utf8char*: println_utf8char          \
             , utf8char: println_move_utf8char            \
        )(v)

/// Prints the given value in a programmer's view, for debugging
/// purposes.

/// Returns an `int` that when negative signifies an error (check
/// `errno` for the error in this case), and when positive signifies
/// the number of bytes written.

/// See `print` for printing as normal text, not for debugging, and
/// `print_debug_array` for printing arrays.

#define print_debug(v)                                                  \
    _Generic((v)                                                        \
             , char**: print_debug_cstr                                 \
             , char*: print_debug_move_cstr                             \
             , cstr*: print_debug_cstr                                  \
             , cstr: print_debug_move_cstr                              \
             , CStr*: print_debug_CStr                                  \
             , CStr: print_debug_move_CStr                              \
             , String*: print_debug_String                              \
             , String: print_debug_move_String                          \
             , bool: print_debug_bool                                   \
             , char: print_debug_char                                   \
             , int*: print_int                                          \
             , int: print_move_int                                      \
             , u8*: print_u8                                            \
             , u8: print_move_u8                                        \
             , u32*: print_u32                                          \
             , u32: print_move_u32                                      \
             , u64*: print_u64                                          \
             , u64: print_move_u64                                      \
             , i64*: print_i64                                          \
             , i64: print_move_i64                                      \
             , float*: print_float                                      \
             , float: print_move_float                                  \
             , double*: print_double                                    \
             , double: print_move_double                                \
             , utf8char*: print_debug_utf8char                          \
             , utf8char: print_debug_move_utf8char                      \
             , ucodepoint*: print_debug_ucodepoint                      \
             , ucodepoint: print_debug_move_ucodepoint                  \
             , Vec2(int): print_debug_move_Vec2_int               \
             , Vec3(int): print_debug_move_Vec3_int               \
             , Vec2(int)*: print_debug_Vec2_int                   \
             , Vec3(int)*: print_debug_Vec3_int                   \
             , Vec2(float): print_debug_move_Vec2_float               \
             , Vec3(float): print_debug_move_Vec3_float               \
             , Vec2(float)*: print_debug_Vec2_float                   \
             , Vec3(float)*: print_debug_Vec3_float                   \
             , Vec2(double): print_debug_move_Vec2_double               \
             , Vec3(double): print_debug_move_Vec3_double               \
             , Vec2(double)*: print_debug_Vec2_double                   \
             , Vec3(double)*: print_debug_Vec3_double                   \
             , Vec2(u32): print_debug_move_Vec2_u32               \
             , Vec3(u32): print_debug_move_Vec3_u32               \
             , Vec2(u32)*: print_debug_Vec2_u32                   \
             , Vec3(u32)*: print_debug_Vec3_u32                   \
             , Vec(cstr): print_debug_move_Vec_cstr                     \
             , Vec(cstr)*: print_debug_Vec_cstr                         \
             , Vec(CStr): print_debug_move_Vec_CStr                     \
             , Vec(CStr)*: print_debug_Vec_CStr                         \
             , Vec(ucodepoint): print_debug_move_Vec_ucodepoint         \
             , Vec(ucodepoint)*: print_debug_Vec_ucodepoint             \
             , Vec(utf8char): print_debug_move_Vec_utf8char             \
             , Vec(utf8char)*: print_debug_Vec_utf8char                 \
             , Vec(char): print_debug_move_Vec_char                     \
             , Vec(char)*: print_debug_Vec_char                         \
             , Vec(int): print_debug_move_Vec_int                       \
             , Vec(int)*: print_debug_Vec_int                           \
             , Vec(Vec2(int)): print_debug_move_Vec_Vec2_int            \
             , Vec(Vec2(int))*: print_debug_Vec_Vec2_int                \
             , Vec(Vec2(float)): print_debug_move_Vec_Vec2_float            \
             , Vec(Vec2(float))*: print_debug_Vec_Vec2_float                \
             , Vec(Rect2(float)): print_debug_move_Vec_Rect2_float            \
             , Vec(Rect2(float))*: print_debug_Vec_Rect2_float                \
             , Vec(float): print_debug_move_Vec_float                 \
             , Vec(float)*: print_debug_Vec_float                     \
             , Vec(double): print_debug_move_Vec_double              \
             , Vec(double)*: print_debug_Vec_double                  \
             , mutslice(cstr): print_debug_move_mutslice_cstr           \
             , mutslice(cstr)*: print_debug_mutslice_cstr               \
             , mutslice(CStr): print_debug_move_mutslice_CStr           \
             , mutslice(CStr)*: print_debug_mutslice_CStr               \
             , mutslice(ucodepoint): print_debug_move_mutslice_ucodepoint \
             , mutslice(ucodepoint)*: print_debug_mutslice_ucodepoint   \
             , mutslice(utf8char): print_debug_move_mutslice_utf8char   \
             , mutslice(utf8char)*: print_debug_mutslice_utf8char       \
             , mutslice(char): print_debug_move_mutslice_char           \
             , mutslice(char)*: print_debug_mutslice_char               \
             , mutslice(int): print_debug_move_mutslice_int             \
             , mutslice(int)*: print_debug_mutslice_int                 \
             , mutslice(Vec2(int)): print_debug_move_mutslice_Vec2_int      \
             , mutslice(Vec2(int))*: print_debug_mutslice_Vec2_int          \
             , slice(cstr): print_debug_move_slice_cstr                 \
             , slice(cstr)*: print_debug_slice_cstr                     \
             , slice(CStr): print_debug_move_slice_CStr                 \
             , slice(CStr)*: print_debug_slice_CStr                     \
             , slice(ucodepoint): print_debug_move_slice_ucodepoint     \
             , slice(ucodepoint)*: print_debug_slice_ucodepoint         \
             , slice(utf8char): print_debug_move_slice_utf8char         \
             , slice(utf8char)*: print_debug_slice_utf8char             \
             , slice(char): print_debug_move_slice_char                 \
             , slice(char)*: print_debug_slice_char                     \
             , slice(int): print_debug_move_slice_int                   \
             , slice(int)*: print_debug_slice_int                       \
             , slice(Vec2(int)): print_debug_move_slice_Vec2_int            \
             , slice(Vec2(int))*: print_debug_slice_Vec2_int                    \
             , slice(Vec2(float)): print_debug_move_slice_Vec2_float            \
             , slice(Vec2(float))*: print_debug_slice_Vec2_float                \
             , slice(Rect2(float)): print_debug_move_slice_Rect2_float            \
             , slice(Rect2(float))*: print_debug_slice_Rect2_float                \
             , Option(cstr)*: print_debug_Option_cstr                   \
             , Option(cstr): print_debug_move_Option_cstr               \
             , Option(CStr)*: print_debug_Option_CStr                   \
             , Option(CStr): print_debug_move_Option_CStr               \
             , Option(String)*: print_debug_Option_String               \
             , Option(String): print_debug_move_Option_String           \
             , Option(int)*: print_debug_Option_int                     \
             , Option(int): print_debug_move_Option_int                 \
             , Option(float)*: print_debug_Option_float                 \
             , Option(float): print_debug_move_Option_float             \
             , Rect2(int)*: print_debug_Rect2_int                       \
             , Rect2(int): print_debug_move_Rect2_int                   \
             , Rect2(float)*: print_debug_Rect2_float                   \
             , Rect2(float): print_debug_move_Rect2_float               \
             , Rect2(double)*: print_debug_Rect2_double                 \
             , Rect2(double): print_debug_move_Rect2_double             \
             , Result(int, ParseError)*: print_debug_Result_int__ParseError \
             , Result(int, ParseError): print_debug_move_Result_int__ParseError \
             , Result(String, SystemError): print_debug_move_Result_String__SystemError \
             , SystemError: print_debug_SystemError                     \
             , UnicodeError*: print_debug_UnicodeError                  \
             , Color*: print_debug_Color                                \
             , Color: print_debug_move_Color                            \
             , VertexRenderer*: print_debug_VertexRenderer               \
             , const cstr*: print_debug_cstr                                  \
             , const CStr*: print_debug_CStr                                  \
             , const String*: print_debug_String                              \
             , const int*: print_int                                          \
             , const u8*: print_u8                                            \
             , const u32*: print_u32                                          \
             , const u64*: print_u64                                          \
             , const i64*: print_i64                                          \
             , const float*: print_float                                      \
             , const double*: print_double                                    \
             , const utf8char*: print_debug_utf8char                          \
             , const ucodepoint*: print_debug_ucodepoint                      \
             , const Vec(cstr)*: print_debug_Vec_cstr                         \
             , const Vec(CStr)*: print_debug_Vec_CStr                         \
             , const Vec(ucodepoint)*: print_debug_Vec_ucodepoint             \
             , const Vec(utf8char)*: print_debug_Vec_utf8char                 \
             , const Vec(char)*: print_debug_Vec_char                         \
             , const Vec(int)*: print_debug_Vec_int                           \
             , const Vec(float)*: print_debug_Vec_float               \
             , const Vec(double)*: print_debug_Vec_double            \
             , const mutslice(cstr)*: print_debug_mutslice_cstr               \
             , const mutslice(CStr)*: print_debug_mutslice_CStr               \
             , const mutslice(ucodepoint)*: print_debug_mutslice_ucodepoint   \
             , const mutslice(utf8char)*: print_debug_mutslice_utf8char       \
             , const mutslice(char)*: print_debug_mutslice_char               \
             , const mutslice(int)*: print_debug_mutslice_int                 \
             , const slice(cstr)*: print_debug_slice_cstr                     \
             , const slice(CStr)*: print_debug_slice_CStr                     \
             , const slice(ucodepoint)*: print_debug_slice_ucodepoint         \
             , const slice(utf8char)*: print_debug_slice_utf8char             \
             , const slice(char)*: print_debug_slice_char                     \
             , const slice(int)*: print_debug_slice_int                       \
             , const Option(cstr)*: print_debug_Option_cstr                   \
             , const Option(CStr)*: print_debug_Option_CStr                   \
             , const Option(String)*: print_debug_Option_String               \
             , const Option(int)*: print_debug_Option_int                     \
             , const Option(float)*: print_debug_Option_float                 \
             , const Result(int, ParseError)*: print_debug_Result_int__ParseError \
             , const UnicodeError*: print_debug_UnicodeError                  \
             , const Color*: print_debug_Color                                \
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
             , cstr*: print_debug_cstrs                         \
             , Option(String)*: print_debug_Option_Strings      \
             , int*: print_debug_ints                           \
             , float*: print_debug_floats                       \
        )((v), (len))



/// Frees the resources held by the given value (including resources
/// held by contained values, recursively, unlike the `free` function).
#define drop(v)                                                  \
    _Generic((v)                                                 \
             , CStr: drop_CStr                                   \
             , String: drop_String                               \
             , SystemError: drop_SystemError                     \
             , ParseError: drop_ParseError                       \
             , CStrError: drop_CStrError                         \
             , VecError: drop_VecError                           \
             , UnicodeError: drop_UnicodeError                   \
             , CFile: drop_CFile                                 \
             , const char*: drop_cstr                            \
             , char*: drop_cstr                                  \
             , int*: free                                        \
             , float*: free                                      \
             , Vec2(int)*: free                               \
             , Vec3(int)*: free                                       \
             , Vec2(float)*: free                               \
             , Vec3(float)*: free                                       \
             , Vec2(double)*: free                               \
             , Vec3(double)*: free                                       \
             , ucodepoint: drop_ucodepoint                       \
             , Vec(cstr): drop_Vec_cstr                          \
             , Vec(CStr): drop_Vec_CStr                          \
             , Vec(ucodepoint): drop_Vec_ucodepoint              \
             , Vec(utf8char): drop_Vec_utf8char                  \
             , Vec(char): drop_Vec_char                          \
             , Vec(int): drop_Vec_int                            \
             , Vec(Vec2(int)): drop_Vec_Vec2_int                         \
             , Vec(Vec2(float)): drop_Vec_Vec2_float                         \
             , Vec(Rect2(float)): drop_Vec_Rect2_float                         \
             , Vec(Vec2(double)): drop_Vec_Vec2_double                         \
             , Vec(float): drop_Vec_float                            \
             , Vec(double): drop_Vec_double                          \
             , mutslice(cstr): drop_mutslice_cstr                \
             , mutslice(CStr): drop_mutslice_CStr                \
             , mutslice(ucodepoint): drop_mutslice_ucodepoint    \
             , mutslice(utf8char): drop_mutslice_utf8char        \
             , slice(cstr): drop_slice_cstr                      \
             , slice(CStr): drop_slice_CStr                      \
             , slice(ucodepoint): drop_slice_ucodepoint          \
             , slice(utf8char): drop_slice_utf8char              \
             , Option(cstr): drop_Option_cstr                    \
             , Option(CStr): drop_Option_CStr                    \
             , Option(String): drop_Option_String                \
             , Option(int): drop_Option_int                      \
             , Option(float): drop_Option_float                  \
             , Option(double): drop_Option_double                \
             , VertexRenderer: drop_VertexRenderer               \
        )(v)



#define RESIZE_ARRAY_test_equal(ary, oldlen, newlen)    \
    if (newlen == oldlen) {                             \
        return ary;                                     \
    }
#define RESIZE_ARRAY_free(ary, oldlen, newlen)          \
    if (newlen < oldlen) {                              \
        for (size_t i = newlen; i < oldlen; i++) {      \
            if (ary[i]) { free((void*)ary[i]); } /* todo don't hack, don't use `cstr` */ \
        }                                               \
    }
#define RESIZE_ARRAY_drop(ary, oldlen, newlen)          \
    if (newlen < oldlen) {                              \
        for (size_t i = newlen; i < oldlen; i++) {      \
            drop(ary[i]);                               \
        }                                               \
    }
#define RESIZE_ARRAY_realloc_and_fill(ary, oldlen, newlen, T, null)     \
    T* res = xreallocarray(ary, newlen, sizeof(T));                     \
    if (newlen > oldlen) {                                              \
        for (size_t i = oldlen; i < newlen; i++) {                      \
            res[i] = null;                                              \
        }                                                               \
    }                                                                   \
    return res;


static UNUSED
cstr* resize_cstrs(cstr* ary, size_t oldlen, size_t newlen) {
    RESIZE_ARRAY_test_equal(ary, oldlen, newlen);
    RESIZE_ARRAY_free(ary, oldlen, newlen);
    RESIZE_ARRAY_realloc_and_fill(ary, oldlen, newlen, cstr, NULL);
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
             , cstr*: resize_cstrs                      \
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
             , cstr*: drop_cstrs                        \
             , Option(String)*: drop_Option_Strings     \
        )((v), (len))


/// Takes two values of the same type and returns a `bool`. Returns
/// `true` if `a` and `b` are structurally equivalent.
#define equal(a, b)                                                     \
    _Generic((a)                                                        \
             , Option(cstr)*: equal_Option_cstr                         \
             , Option(String)*: equal_Option_String                     \
             , Option(int)*: equal_Option_int                           \
             , Option(float)*: equal_Option_float                       \
             , Option(char)*: equal_Option_char                         \
             , Option(char): equal_move_Option_char                     \
             , Option(utf8char)*: equal_Option_utf8char                 \
             , Option(utf8char): equal_move_Option_utf8char             \
             , Option(ucodepoint)*: equal_Option_ucodepoint             \
             , Option(ucodepoint): equal_move_Option_ucodepoint         \
             , utf8char*: equal_utf8char                                \
             , utf8char: equal_move_utf8char                            \
             , ucodepoint*: equal_ucodepoint                            \
             , ucodepoint: equal_move_ucodepoint                        \
             , Result(int, ParseError)*: equal_Result_int__ParseError   \
             , Result(String, SystemError)*: equal_Result_String__SystemError \
             , cstr*: equal_cstr                                        \
             , CStr*: equal_CStr                                        \
             , String*: equal_String                                    \
             , cstr: equal_move_cstr                                    \
             , int*: equal_int                                          \
             , int: equal_move_int                                      \
             , float*: equal_float                                      \
             , float: equal_move_float                                  \
             , double*: equal_double                                    \
             , double: equal_move_double                                \
             , bool*: equal_bool                                        \
             , bool: equal_move_bool                                    \
             , char: equal_move_char                                    \
             , Color*: equal_Color                                      \
             , Color: equal_move_Color                                  \
             , Vec(cstr)*: equal_Vec_cstr                               \
             , Vec(CStr)*: equal_Vec_CStr                               \
             , Vec(ucodepoint)*: equal_Vec_ucodepoint                   \
             , Vec(utf8char)*: equal_Vec_utf8char                       \
             , Vec(char)*: equal_Vec_char                               \
             , Vec(int)*: equal_Vec_int                                 \
             , Vec(Vec2(int))*: equal_Vec_Vec2_int                       \
             , Vec(Vec2(float))*: equal_Vec_Vec2_float                       \
             , Vec(Rect2(float))*: equal_Vec_Rect2_float                       \
             , Vec(Vec2(double))*: equal_Vec_Vec2_double                       \
             , Vec(float)*: equal_Vec_float                             \
             , Vec(double)*: equal_Vec_double                           \
             , slice(cstr)*: equal_slice_cstr                               \
             , slice(CStr)*: equal_slice_CStr                               \
             , slice(ucodepoint)*: equal_slice_ucodepoint                   \
             , slice(utf8char)*: equal_slice_utf8char                       \
             , slice(char)*: equal_slice_char                               \
             , slice(int)*: equal_slice_int                                 \
             , slice(Vec2(int))*: equal_slice_Vec2_int                       \
             , slice(Vec2(float))*: equal_slice_Vec2_float                       \
             , slice(Rect2(float))*: equal_slice_Rect2_float                       \
             , slice(Vec2(double))*: equal_slice_Vec2_double                       \
             , slice(float)*: equal_slice_float                             \
             , slice(double)*: equal_slice_double                           \
             , const Option(cstr)*: equal_Option_cstr                         \
             , const Option(String)*: equal_Option_String                     \
             , const Option(int)*: equal_Option_int                           \
             , const Option(float)*: equal_Option_float                       \
             , const Option(char)*: equal_Option_char                         \
             , const Option(utf8char)*: equal_Option_utf8char                 \
             , const Option(ucodepoint)*: equal_Option_ucodepoint             \
             , const utf8char*: equal_utf8char                                \
             , const ucodepoint*: equal_ucodepoint                            \
             , const Result(int, ParseError)*: equal_Result_int__ParseError   \
             , const Result(String, SystemError)*: equal_Result_String__SystemError \
             , const cstr*: equal_cstr                                        \
             , const CStr*: equal_CStr                                        \
             , const String*: equal_String                                    \
             , const int*: equal_int                                          \
             , const float*: equal_float                                      \
             , const double*: equal_double                                    \
             , const bool*: equal_bool                                        \
             , const Color*: equal_Color                                      \
             , const Vec(cstr)*: equal_Vec_cstr                               \
             , const Vec(CStr)*: equal_Vec_CStr                               \
             , const Vec(ucodepoint)*: equal_Vec_ucodepoint                   \
             , const Vec(utf8char)*: equal_Vec_utf8char                       \
             , const Vec(char)*: equal_Vec_char                         \
             , const Vec(int)*: equal_Vec_int                           \
             , const Vec(float)*: equal_Vec_float                         \
             , const Vec(double)*: equal_Vec_double                         \
        )((a), (b))

/* /// Call `equal(&a, &b)` but works even if `a` or `b` are expressions */
/* /// that are not variables or data structure slots. */
/* #define EQUAL(a, b)                                             \ */
/*     ({                                                          \ */
/*         typeof(a) HYGIENIC(a) = (a);                            \ */
/*         typeof(b) HYGIENIC(b) = (b);                            \ */
/*         bool HYGIENIC(res) = equal(&HYGIENIC(a), &HYGIENIC(b)); \ */
/*         drop(HYGIENIC(a));                                      \ */
/*         drop(HYGIENIC(b));                                      \ */
/*         HYGIENIC(res);                                          \ */
/*     }) */
// This can't ever work.


/// Takes a value of some type `T` and returns a `some` variant of
/// `Option(T)` containing the value.
#define some(v)                                         \
    _Generic((v)                                        \
             , cstr: some_cstr                          \
             , CStr: some_CStr                          \
             , String: some_String                      \
             , int: some_int                            \
             , u8: some_u8                              \
             , u64: some_u64                            \
             , i64: some_i64                            \
             , float: some_float                        \
             , double: some_double                      \
             , Vec2(float): some_Vec2_float             \
        )(v)

/// Returns the none variant of `Option(T)` for the given
/// T. Equivalent to `none_T()` using the type name of T, but
/// `none(T)` allows T to also be a `typeof` expression.
#define none(T)                                         \
    _Generic(*((T*)(NULL))                              \
             , cstr: none_cstr                          \
             , CStr: none_CStr                          \
             , String: none_String                      \
             , int: none_int                            \
             , u8: none_u8                              \
             , u64: none_u64                            \
             , i64: none_i64                            \
             , float: none_float                        \
             , double: none_double                      \
        )()

/// Takes a wrapper for some type `T` (like `Option(T)`, or `Result(T,
/// E)`) and returns the contained value of type `T` if possible, or
/// aborts if not possible. This moves the value, i.e. the wrapper
/// is consumed.

/// Example:
///
/// ```C
/// Option(CStr) maybe_name = get_CStr();
/// // maybe_name can be a `none(CStr)` or e.g. `some("Alex")`.
/// CStr name = unwrap(maybe_name);
/// // Now we're guaranteed to have a CStr in `name`, "Alex" if we
/// // had the second case above. But if `maybe_name` was a none
/// // then the program has terminated by now instead.
/// ```

#define unwrap(v)                                                       \
    _Generic((v)                                                        \
             , Option(cstr): unwrap_Option_cstr                         \
             , Option(CStr): unwrap_Option_CStr                         \
             , Option(String): unwrap_Option_String                     \
             , Option(char): unwrap_Option_char                         \
             , Option(utf8char): unwrap_Option_utf8char                 \
             , Option(ucodepoint): unwrap_Option_ucodepoint             \
             , Option(int): unwrap_Option_int                           \
             , Option(u8): unwrap_Option_u8                             \
             , Option(u64): unwrap_Option_u64                           \
             , Option(i64): unwrap_Option_i64                           \
             , Option(float): unwrap_Option_float                       \
             , Option(double): unwrap_Option_double                     \
             , Result(int, ParseError): unwrap_Result_int__ParseError   \
             , Result(float, ParseError): unwrap_Result_float__ParseError   \
             , Result(String, SystemError): unwrap_Result_String__SystemError \
             , Result(Unit, UnicodeError): unwrap_Result_Unit__UnicodeError \
             , Result(ucodepoint, UnicodeError): unwrap_Result_ucodepoint__UnicodeError \
             , Result(size_t, UnicodeError): unwrap_Result_size_t__UnicodeError \
             , Result(Vec(utf8char), UnicodeError): unwrap_Result_Vec_utf8char__UnicodeError \
             , Result(Vec(ucodepoint), UnicodeError): unwrap_Result_Vec_ucodepoint__UnicodeError \
             , Result(String, UnicodeError): unwrap_Result_String__UnicodeError \
        )(v)


/// Takes a wrapper for some type `T` (like `Option(T)`, or `Result(T,
/// E)`) and returns the contained value of type `T` if possible, or
/// aborts if not possible. This moves the value, i.e. the wrapper
/// is consumed.

/// Example:
///
/// ```C
/// Option(CStr) maybe_name = get_CStr();
/// // maybe_name can be a `none(CStr)` or e.g. `some("Alex")`.
/// CStr name = unwrap_or(maybe_name, CStr_from_cstr_unsafe("no name"));
/// // Now we're guaranteed to have a CStr in `name`, "Alex" if we
/// // had the second case above, "no name" in the first.
/// ```

#define unwrap_or(v)                                                    \
    _Generic((v)                                                        \
             , Option(cstr): unwrap_or_Option_cstr                         \
             , Option(CStr): unwrap_or_Option_CStr                         \
             , Option(String): unwrap_or_Option_String                     \
             , Option(char): unwrap_or_Option_char                         \
             , Option(utf8char): unwrap_or_Option_utf8char                 \
             , Option(ucodepoint): unwrap_or_Option_ucodepoint             \
             , Option(int): unwrap_or_Option_int                           \
             , Option(u8): unwrap_or_Option_u8                             \
             , Option(u64): unwrap_or_Option_u64                           \
             , Option(i64): unwrap_or_Option_i64                           \
             , Option(float): unwrap_or_Option_float                       \
             , Option(double): unwrap_or_Option_double                     \
             , Result(int, ParseError): unwrap_or_Result_int__ParseError   \
             , Result(float, ParseError): unwrap_or_Result_float__ParseError   \
             , Result(String, SystemError): unwrap_or_Result_String__SystemError \
             , Result(Unit, UnicodeError): unwrap_or_Result_Unit__UnicodeError \
             , Result(ucodepoint, UnicodeError): unwrap_or_Result_ucodepoint__UnicodeError \
             , Result(size_t, UnicodeError): unwrap_or_Result_size_t__UnicodeError \
             , Result(Vec(utf8char), UnicodeError): unwrap_or_Result_Vec_utf8char__UnicodeError \
             , Result(Vec(ucodepoint), UnicodeError): unwrap_or_Result_Vec_ucodepoint__UnicodeError \
             , Result(String, UnicodeError): unwrap_or_Result_String__UnicodeError \
        )(v)

/// Dereferencing a wrapper type ('smart pointer') to the simpler
/// contained type; the result is of a reference type (not owned).

/// Example:
///
/// ```C
/// CStr s = unwrap(filecontents_CStr(path));
/// cstr s1 = deref(&s);
/// ```

#define deref(v)                                                        \
    _Generic((v)                                                        \
             , CStr*: deref_CStr                                        \
             , Vec(Vec2(int))*: deref_Vec_Vec2_int                  \
             , Vec(Vec2(float))*: deref_Vec_Vec2_float                  \
             , Vec(Rect2(float))*: deref_Vec_Rect2_float                  \
             , Vec(Vec2(double))*: deref_Vec_Vec2_double                  \
        )(v)


/// Returns the name of the type of `e` as a string constant (cstr).
#define type_name(e)                                                    \
    _Generic((e)                                                        \
             , cstr: "cstr"                                             \
             , char*: "char*"                                           \
             , String: "String"                                         \
             , int: "int"                                               \
             , unsigned int: "uint"                                     \
             , u8: "u8"                                                 \
             , u64: "u64"                                               \
             , i64: "i64"                                               \
             , float: "float"                                           \
             , double: "double"                                         \
             , ucodepoint: "ucodepoint"                                 \
             , Option(cstr): "Option(cstr)"                             \
             , Option(String): "Option(String)"                         \
             , Option(int): "Option(int)"                               \
             , Option(u8): "Option(u8)"                                 \
             , Option(u64): "Option(u64)"                               \
             , Option(i64): "Option(i64)"                               \
             , Option(float): "Option(float)"                           \
             , Option(double): "Option(double)"                         \
             , Vec(cstr): "Vec(cstr)"                                   \
             , Vec(CStr): "Vec(CStr)"                                   \
             , Vec(ucodepoint): "Vec(ucodepoint)"                       \
             , Vec(utf8char): "Vec(utf8char)"                           \
             , Vec(char): "Vec(char)"                                   \
             , Vec(int): "Vec(int)"                                     \
             , Vec(float): "Vec(float)"                                   \
             , Vec(double): "Vec(double)"                                   \
             , mutslice(cstr): "mutslice(cstr)"                         \
             , mutslice(CStr): "mutslice(CStr)"                         \
             , mutslice(ucodepoint): "mutslice(ucodepoint)"             \
             , mutslice(utf8char): "mutslice(utf8char)"                 \
             , slice(cstr): "slice(cstr)"                               \
             , slice(CStr): "slice(CStr)"                               \
             , slice(ucodepoint): "slice(ucodepoint)"                   \
             , slice(utf8char): "slice(utf8char)"                       \
             , Rect2(int): "Rect2(int)"                                 \
             , Rect2(float): "Rect2(float)"                             \
             , Rect2(double): "Rect2(double)"                           \
             , Result(int, ParseError): "Result(int, ParseError)"       \
             , Result(String, SystemError): "Result(String, SystemError)" \
        )


GENERATE_equal_array(char);
GENERATE_equal_array(cstr);
GENERATE_equal_array(String);
GENERATE_equal_array(int);
GENERATE_equal_array(uint);
GENERATE_equal_array(u64);
GENERATE_equal_array(i64);
GENERATE_equal_array(float);
GENERATE_equal_array(double);

/// Returns true iff len1 == len2 and for every index,
/// `equal(array1[i], array2[i])` is true.
#define equal_array(array1, len1, array2, len2)         \
    _Generic((array1)[0]                                \
             , char: equal_array_char                   \
             , cstr: equal_array_cstr                   \
             , String: equal_array_String               \
             , int: equal_array_int                     \
             , uint: equal_array_uint                   \
             , u64: equal_array_u64                     \
             , i64: equal_array_i64                     \
             , float: equal_array_float                 \
             , double: equal_array_double               \
        )((array1), (len1), (array2), (len2))


// These are documented in Result.h -- removed now
// #define ok(val, E) ...
// #define err(val, E) ...


/// "DeBuG": print the expression `expr` and the value it evaluated to, for
/// debugging purposes (calls `print_debug` on the value). Returns the value,
/// although note that this is only valid if the value is of a Copy type
/// (including references). See `DBGV` for a variant that handles non-Copy types
/// that way.
#define DBG(expr)                               \
    ({                                          \
        __typeof__ (expr) HYGIENIC(v) = (expr); \
        print("DEBUG: " #expr " == ");          \
        print_debug(HYGIENIC(v));               \
        print("\n");                            \
        HYGIENIC(v);                            \
    })

/// "DeBuG (non-reference) value": print the expression `expr` and the value it
/// evaluated to, for debugging purposes (calls `print_debug` on the reference
/// to the value). Returns the value; unlike `DBG`, this does not move the
/// value, hence the return value of `DBGV` *must* be used if it is not a Copy
/// type (`DBGV` never drops it). OTOH, `DBGV` can't be used on references,
/// because that would lead to a double reference being passed to `print_debug`;
/// use `DBG` for those.

/// Is expr evaluating to a reference? -> use `DBG`. Otherwise, do you want to
/// pick up the return value? -> use `DBGV`. Otherwise use `DBG`.
#define DBGV(expr)                              \
    ({                                          \
        __typeof__ (expr) HYGIENIC(v) = (expr); \
        print("DEBUG: " #expr " == ");          \
        print_debug(&HYGIENIC(v));              \
        print("\n");                            \
        HYGIENIC(v);                            \
    })


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
        RESRET(print(s));
    } else {
        RESRET(print("("));
        RESRET(print(s));
        RESRET(print(")"));
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


/// Push a value onto the end of a collection. For Vec and String.
#define push(coll, val)                                 \
    _Generic((coll)                                     \
             , Vec(cstr)*: push_Vec_cstr                \
             , Vec(CStr)*: push_Vec_CStr                \
             , Vec(ucodepoint)*: push_Vec_ucodepoint    \
             , Vec(utf8char)*: push_Vec_utf8char        \
             , Vec(char)*: push_Vec_char                \
             , Vec(int)*: push_Vec_int                  \
             , Vec(Vec2(int))*: push_Vec_Vec2_int       \
             , Vec(Vec2(float))*: push_Vec_Vec2_float   \
             , Vec(Rect2(float))*: push_Vec_Rect2_float \
             , Vec(Vec2(double))*: push_Vec_Vec2_double \
             , Vec(float)*: push_Vec_float              \
             , Vec(double)*: push_Vec_double            \
             , String*: push_String                	\
        )(coll, val)


/// Pop a value off the end of a collection. For Vec and String.
#define pop(coll)                                       \
    _Generic((coll)                                     \
             , Vec(cstr)*: pop_Vec_cstr                 \
             , Vec(CStr)*: pop_Vec_CStr                 \
             , Vec(ucodepoint)*: pop_Vec_ucodepoint     \
             , Vec(utf8char)*: pop_Vec_utf8char         \
             , Vec(char)*: pop_Vec_char                 \
             , Vec(int)*: pop_Vec_int                   \
             , Vec(Vec2(int))*: pop_Vec_Vec2_int           \
             , Vec(Vec2(float))*: pop_Vec_Vec2_float           \
             , Vec(Rect2(float))*: pop_Vec_Rect2_float           \
             , Vec(Vec2(double))*: pop_Vec_Vec2_double           \
             , Vec(float)*: pop_Vec_float                 \
             , Vec(double)*: pop_Vec_double                 \
        )(coll)


/// Append the items from coll2 onto the end of coll1, leaving coll2
/// empty. For Vec and String.
#define append(coll1, coll2)                            \
    _Generic((coll1)                                    \
             , Vec(cstr)*: append_Vec_cstr              \
             , Vec(CStr)*: append_Vec_CStr              \
             , Vec(ucodepoint)*: append_Vec_ucodepoint  \
             , Vec(utf8char)*: append_Vec_utf8char      \
             , Vec(char)*: append_Vec_char              \
             , Vec(int)*: append_Vec_int                \
             , Vec(Vec2(int))*: append_Vec_Vec2_int        \
             , Vec(Vec2(float))*: append_Vec_Vec2_float        \
             , Vec(Rect2(float))*: append_Vec_Rect2_float                     \
             , Vec(Vec2(double))*: append_Vec_Vec2_double        \
             , Vec(float)*: append_Vec_float              \
             , Vec(double)*: append_Vec_double              \
             , String*: append_String_String            \
        )(coll1, coll2)

/// Append the items from coll2 onto the end of coll1, consuming
/// coll2. For Vec and String.
#define append_move(coll1, coll2)                       \
    _Generic((coll1)                                    \
             , String*: append_move_String_String       \
             , Vec(cstr)*: append_move_Vec_cstr         \
             , Vec(CStr)*: append_move_Vec_CStr         \
             , Vec(int)*: append_move_Vec_int         \
             , Vec(Vec2(int))*: append_move_Vec_Vec2_int        \
             , Vec(Vec2(float))*: append_move_Vec_Vec2_float        \
             , Vec(Rect2(float))*: append_move_Vec_Rect2_float                     \
             , Vec(Vec2(double))*: append_move_Vec_Vec2_double        \
             , Vec(float)*: append_move_Vec_float         \
             , Vec(double)*: append_move_Vec_double         \
             , Vec(ucodepoint)*: append_move_Vec_ucodepoint         \
        )(coll1, coll2)


/// Give the length of a given collection. It always reports the
/// number of identically-sized storage locations, which means that
/// for String, it reports the number of bytes (C char), not the
/// number of unicode codepoints.
#define len(coll)                                                 \
    _Generic((coll)                                               \
             , Vec(cstr)*: len_Vec_cstr                           \
             , mutslice(cstr)*: len_mutslice_cstr                 \
             , slice(cstr)*: len_slice_cstr                       \
             , Vec(CStr)*: len_Vec_CStr                           \
             , mutslice(CStr)*: len_mutslice_CStr                 \
             , slice(CStr)*: len_slice_CStr                       \
             , Vec(ucodepoint)*: len_Vec_ucodepoint               \
             , mutslice(ucodepoint)*: len_mutslice_ucodepoint     \
             , slice(ucodepoint)*: len_slice_ucodepoint           \
             , Vec(utf8char)*: len_Vec_utf8char                   \
             , mutslice(utf8char)*: len_mutslice_utf8char         \
             , slice(utf8char)*: len_slice_utf8char               \
             , Vec(char)*: len_Vec_char                           \
             , mutslice(char)*: len_mutslice_char                 \
             , slice(char)*: len_slice_char                       \
             , Vec(int)*: len_Vec_int                             \
             , Vec(Vec2(int))*: len_Vec_Vec2_int                     \
             , Vec(Vec2(float))*: len_Vec_Vec2_float                     \
             , Vec(Rect2(float))*: len_Vec_Rect2_float                     \
             , Vec(Vec2(double))*: len_Vec_Vec2_double                     \
             , Vec(float)*: len_Vec_float                           \
             , Vec(double)*: len_Vec_double                           \
             , mutslice(int)*: len_mutslice_int                   \
             , slice(int)*: len_slice_int                         \
             , String*: len_String                                \
             , const Vec(cstr)*: len_Vec_cstr                           \
             , const mutslice(cstr)*: len_mutslice_cstr                 \
             , const slice(cstr)*: len_slice_cstr                       \
             , const Vec(CStr)*: len_Vec_CStr                           \
             , const mutslice(CStr)*: len_mutslice_CStr                 \
             , const slice(CStr)*: len_slice_CStr                       \
             , const Vec(ucodepoint)*: len_Vec_ucodepoint               \
             , const mutslice(ucodepoint)*: len_mutslice_ucodepoint     \
             , const slice(ucodepoint)*: len_slice_ucodepoint           \
             , const Vec(utf8char)*: len_Vec_utf8char                   \
             , const mutslice(utf8char)*: len_mutslice_utf8char         \
             , const slice(utf8char)*: len_slice_utf8char               \
             , const Vec(char)*: len_Vec_char                           \
             , const mutslice(char)*: len_mutslice_char                 \
             , const slice(char)*: len_slice_char                       \
             , const Vec(int)*: len_Vec_int                             \
             , const Vec(float)*: len_Vec_float                           \
             , const Vec(double)*: len_Vec_double                           \
             , const mutslice(int)*: len_mutslice_int                   \
             , const slice(int)*: len_slice_int                         \
             , const String*: len_String                                \
        )(coll)

/// Give a cstr to the given character collection. Note that this may
/// return an error if the collection contains `'\0'` bytes.
#define cstr(v)                                         \
    _Generic((v)                                        \
             , CStr*: cstr_CStr                         \
             , utf8char*: cstr_utf8char                 \
             , String*: cstr_String                     \
        )(v)


/// Like println but printing to `out`.

/// (Currently only defined for types for which `unwrap` is defined,
/// to be used for error messaging. Todo: define `println` in terms of
/// `fprintln` and thus share implementations.)

#define fprintln(out, v)                                        \
    _Generic((v)                                                \
             , String*: fprintln_String                         \
             , ParseError*: fprintln_ParseError                 \
             , CStrError*: fprintln_CStrError                   \
             , VecError*: fprintln_VecError                     \
             , SystemError*: fprintln_SystemError               \
             , DecodingError*: fprintln_DecodingError           \
             , UnicodeError*: fprintln_UnicodeError             \
        )((out), (v))

/// Get a `utf8char` from a collection. DEPRECATED, use get_ucodepoint
/// instead.

#define get_utf8char(s, idx)                                    \
    _Generic((s)                                                \
             , String*: get_utf8char_String                     \
        )((s), (idx))

/// Get a `ucodepoint` from a collection.

#define get_ucodepoint(s, idx)                                  \
    _Generic((s)                                                \
             , String*: get_ucodepoint_String                   \
        )((s), (idx))


/// Clear a collection or similar thing (make it empty).

#define clear(s)                                                \
    _Generic((s)                                                \
             , SDL_Renderer*: clear_SDL_Renderer /* evil? */    \
             , Vec(cstr)*: clear_Vec_cstr                       \
             , Vec(CStr)*: clear_Vec_CStr                       \
             , Vec(ucodepoint)*: clear_Vec_ucodepoint           \
             , Vec(utf8char)*: clear_Vec_utf8char               \
             , Vec(char)*: clear_Vec_char                   \
             , Vec(int)*: clear_Vec_int                    \
             , Vec(Vec2(int))*: clear_Vec_Vec2_int            \
             , Vec(Vec2(float))*: clear_Vec_Vec2_float            \
             , Vec(Vec2(double))*: clear_Vec_Vec2_double            \
             , Vec(Rect2(float))*: clear_Vec_Rect2_float            \
             , Vec(float)*: clear_Vec_float                   \
             , Vec(double)*: clear_Vec_double                   \
             , String*: clear_String                            \
        )(s)

/// Read items into buf until the delimiter or EOF is reached.

#define read_until(in, delimiter, buf, strip_delimiter, max_len) \
    _Generic((buf),                                              \
             , Vec(ucodepoint)*: read_until_Vec_ucodepoint       \
        )((in), (delimiter), (buf), (strip_delimiter), (max_len))

/// Read items into buf until a newline character (`'\n'`) or EOF is
/// reached.

#define read_line(in, buf, strip_delimiter, max_len)             \
    _Generic((buf),                                              \
             , Vec(ucodepoint)*: read_line_Vec_ucodepoint        \
        )((in), (buf), (strip_delimiter), (max_len))


/// Read an ucodepoint from `in`.

#define get_ucodepoint_unlocked(in)                                     \
    _Generic((in)                                                       \
             , CFile*: get_ucodepoint_unlocked_CFile                    \
             , SliceIterator(char)*: get_ucodepoint_unlocked_SliceIterator_char \
        )(in)

/// Create a String from various types. (Ideally the same as
/// `new_from(String, v)`, but that one has fewer entries.)

#define new_String_from(v)                              \
    _Generic((v)                                        \
             , char*: new_String_from_move_cstr              \
             , cstr: new_String_from_move_cstr               \
             , cstr*: new_String_from_cstr               \
             , const cstr*: new_String_from_cstr               \
             , CStr: new_String_from_CStr               \
             , slice(char): new_String_from_slice_char  \
             , int: new_String_from_move_int            \
             , size_t: new_String_from_move_size_t      \
             , String: new_String_from_String                           \
             , ParseError: new_String_from_ParseError                   \
             , SystemError: new_String_from_SystemError                 \
             , UnicodeError: new_String_from_UnicodeError               \
             , DecodingError: new_String_from_DecodingError             \
        )(v)
// XX Why call it new_, so tempting to call it String_from. Conflict
// with type constructors or not? Also evil or not? Definitely evil,
// but there are other non-new_ constructor macros. Really conflating
// those anyway.


/// Create a `String` easily; this is just an alias for
/// `new_String_from`.

#define String(val)                             \
    new_String_from(val)



/// Get a read-only reference to the element at position `i`. Aborts if `i` is
/// behind the end of the vectorlike. If you are not sure if
/// `i` is valid, use `get` instead.

#define at(v, idx)                                            \
    _Generic((v)                                              \
             , Vec(char)*: at_Vec_char                        \
             , slice(char)*: at_slice_char                    \
             , mutslice(char)*: at_mutslice_char              \
             , Vec(int)*: at_Vec_int                          \
             , slice(int)*: at_slice_int                      \
             , mutslice(int)*: at_mutslice_int                \
             , Vec(cstr)*: at_Vec_cstr                          \
             , slice(cstr)*: at_slice_cstr                      \
             , mutslice(cstr)*: at_mutslice_cstr                \
             , Vec(CStr)*: at_Vec_CStr                          \
             , slice(CStr)*: at_slice_CStr                      \
             , mutslice(CStr)*: at_mutslice_CStr                \
             , Vec(Vec2(int))*: at_Vec_Vec2_int                    \
             , Vec(Vec2(float))*: at_Vec_Vec2_float                    \
             , Vec(Rect2(float))*: at_Vec_Rect2_float                    \
             , Vec(Vec2(double))*: at_Vec_Vec2_double                    \
             , Vec(float)*: at_Vec_float                          \
             , Vec(double)*: at_Vec_double                        \
             , Vec(ucodepoint)*: at_Vec_ucodepoint                        \
             , slice(Vec2(int))*: at_slice_Vec2_int                    \
             , slice(Vec2(float))*: at_slice_Vec2_float                    \
             , slice(Rect2(float))*: at_slice_Rect2_float                    \
             , slice(Vec2(double))*: at_slice_Vec2_double                    \
             , slice(float)*: at_slice_float                          \
             , slice(double)*: at_slice_double                        \
             , slice(ucodepoint)*: at_slice_ucodepoint                        \
             , mutslice(Vec2(int))*: at_mutslice_Vec2_int                    \
             , mutslice(Vec2(float))*: at_mutslice_Vec2_float                    \
             , mutslice(Rect2(float))*: at_mutslice_Rect2_float                    \
             , mutslice(Vec2(double))*: at_mutslice_Vec2_double                    \
             , mutslice(float)*: at_mutslice_float                          \
             , mutslice(double)*: at_mutslice_double                        \
             , mutslice(ucodepoint)*: at_mutslice_ucodepoint                        \
             , const slice(char)*: at_slice_char              \
             , const mutslice(char)*: at_mutslice_char        \
             , const Vec(char)*: at_Vec_char                  \
             , const slice(int)*: at_slice_int                \
             , const mutslice(int)*: at_mutslice_int          \
             , const Vec(int)*: at_Vec_int                    \
             , const Vec(float)*: at_Vec_float                  \
             , const Vec(double)*: at_Vec_double                  \
             , const Vec(ucodepoint)*: at_Vec_ucodepoint                        \
        )((v), (idx))

/// Get a read-only reference to the element at position `i`. Returns none if
/// `i` is behind the end of the vectorlike. If you are sure that `i` is valid,
/// you can use `at` instead.

#define get(v, idx)                                            \
    _Generic((v)                                              \
             , Vec(char)*: get_Vec_char                        \
             , slice(char)*: get_slice_char                    \
             , mutslice(char)*: get_mutslice_char              \
             , Vec(int)*: get_Vec_int                          \
             , slice(int)*: get_slice_int                      \
             , mutslice(int)*: get_mutslice_int                \
             , Vec(cstr)*: get_Vec_cstr                          \
             , slice(cstr)*: get_slice_cstr                      \
             , mutslice(cstr)*: get_mutslice_cstr                \
             , Vec(CStr)*: get_Vec_CStr                          \
             , slice(CStr)*: get_slice_CStr                      \
             , mutslice(CStr)*: get_mutslice_CStr                \
             , Vec(Vec2(int))*: get_Vec_Vec2_int                    \
             , Vec(Vec2(float))*: get_Vec_Vec2_float                    \
             , Vec(Rect2(float))*: get_Vec_Rect2_float                    \
             , Vec(Vec2(double))*: get_Vec_Vec2_double                    \
             , Vec(float)*: get_Vec_float                          \
             , Vec(double)*: get_Vec_double                        \
             , Vec(ucodepoint)*: get_Vec_ucodepoint                        \
             , slice(Vec2(int))*: get_slice_Vec2_int                    \
             , slice(Vec2(float))*: get_slice_Vec2_float                    \
             , slice(Rect2(float))*: get_slice_Rect2_float                    \
             , slice(Vec2(double))*: get_slice_Vec2_double                    \
             , slice(float)*: get_slice_float                          \
             , slice(double)*: get_slice_double                        \
             , slice(ucodepoint)*: get_slice_ucodepoint                        \
             , mutslice(Vec2(int))*: get_mutslice_Vec2_int                    \
             , mutslice(Vec2(float))*: get_mutslice_Vec2_float                    \
             , mutslice(Rect2(float))*: get_mutslice_Rect2_float                    \
             , mutslice(Vec2(double))*: get_mutslice_Vec2_double                    \
             , mutslice(float)*: get_mutslice_float                          \
             , mutslice(double)*: get_mutslice_double                        \
             , mutslice(ucodepoint)*: get_mutslice_ucodepoint                        \
             , const slice(char)*: get_slice_char              \
             , const mutslice(char)*: get_mutslice_char        \
             , const Vec(char)*: get_Vec_char                  \
             , const slice(int)*: get_slice_int                \
             , const mutslice(int)*: get_mutslice_int          \
             , const Vec(int)*: get_Vec_int                    \
             , const Vec(float)*: get_Vec_float                  \
             , const Vec(double)*: get_Vec_double                  \
             , const Vec(ucodepoint)*: get_Vec_ucodepoint                        \
        )((v), (idx))


/// Move the given `val` into the slot with the given `idx`,
/// replacing the value that was there previously. Aborts if the index
/// isn't smaller than the current `len` of the vector.

#define set(v, idx, val)                                      \
    _Generic((v)                                              \
             , mutslice(char)*: set_mutslice_char             \
             , Vec(char)*: set_Vec_char                       \
             , mutslice(int)*: set_mutslice_int               \
             , Vec(int)*: set_Vec_int                         \
             , Vec(Vec2(int))*: set_Vec_Vec2_int                 \
             , Vec(Vec2(float))*: set_Vec_Vec2_float                 \
             , Vec(Vec2(double))*: set_Vec_Vec2_double                 \
             , Vec(float)*: set_Vec_float                       \
             , Vec(double)*: set_Vec_double                       \
             , const mutslice(char)*: set_mutslice_char       \
             , const Vec(char)*: set_Vec_char                 \
             , const mutslice(int)*: set_mutslice_int         \
             , const Vec(int)*: set_Vec_int                   \
             , const Vec(float)*: set_Vec_float                 \
             , const Vec(double)*: set_Vec_double                 \
        )((v), (idx), (val))


/// Create a slice of various collection types. A slice is borrowing a
/// range of items from the original collection. Careful, these
/// "always succeed": if the range is not applicable, they abort.

#define slice_of(v, range)                                    \
    _Generic((v)                                              \
             , slice(char)*: slice_of_slice_char              \
             , mutslice(char)*: slice_of_mutslice_char        \
             , Vec(char)*: slice_of_Vec_char                  \
             , slice(int)*: slice_of_slice_int                \
             , mutslice(int)*: slice_of_mutslice_int          \
             , Vec(int)*: slice_of_Vec_int                    \
             , Vec(Vec2(int))*: slice_of_Vec_Vec2_int            \
             , Vec(Vec2(float))*: slice_of_Vec_Vec2_float            \
             , Vec(Vec2(double))*: slice_of_Vec_Vec2_double            \
             , Vec(float)*: slice_of_Vec_float                  \
             , Vec(double)*: slice_of_Vec_double                  \
             , const slice(char)*: slice_of_slice_char        \
             , const mutslice(char)*: slice_of_mutslice_char  \
             , const Vec(char)*: slice_of_Vec_char            \
             , const slice(int)*: slice_of_slice_int          \
             , const mutslice(int)*: slice_of_mutslice_int    \
             , const Vec(int)*: slice_of_Vec_int              \
             , const Vec(float)*: slice_of_Vec_float            \
             , const Vec(double)*: slice_of_Vec_double            \
        )((v), (range))

/// Create a mutable slice of various collection types. A slice is borrowing a
/// range of items from the original collection. Careful, these
/// "always succeed": if the range is not applicable, they abort.

#define mutslice_of(v, range)                                    \
    _Generic((v)                                                 \
             , mutslice(char)*: mutslice_of_mutslice_char        \
             , Vec(char)*: mutslice_of_Vec_char                  \
             , mutslice(int)*: mutslice_of_mutslice_int          \
             , Vec(int)*: mutslice_of_Vec_int                    \
             , Vec(Vec2(int))*: mutslice_of_Vec_Vec2_int            \
             , Vec(Vec2(float))*: mutslice_of_Vec_Vec2_float            \
             , Vec(Vec2(double))*: mutslice_of_Vec_Vec2_double            \
             , Vec(float)*: mutslice_of_Vec_float                  \
             , Vec(double)*: mutslice_of_Vec_double                  \
             , const mutslice(char)*: mutslice_of_mutslice_char  \
             , const Vec(char)*: mutslice_of_Vec_char            \
             , const mutslice(int)*: mutslice_of_mutslice_int    \
             , const Vec(int)*: mutslice_of_Vec_int              \
             , const Vec(float)*: mutslice_of_Vec_float            \
             , const Vec(double)*: mutslice_of_Vec_double            \
        )((v), (range))
// ^ adding const variants only because that should give better
// compiler error messages.


/// Create a slice of various collection types. A slice is borrowing a
/// range of items from the original collection. These return None if
/// the given range is not applicable.

#define get_slice_of(v, range)                          \
    _Generic((v)                                        \
             , String*: get_slice_of_String             \
             , const String*: get_slice_of_String       \
        )((v), (range))


/// Create a Rect2 (see [cj50/gen/Rect2.h](cj50/gen/Rect2.h.md)).

#define rect2(start, extent)                        \
    _Generic((start)                                \
             , Vec2(int): rect2_int                 \
             , Vec2(float): rect2_float             \
             , Vec2(double): rect2_double           \
        )((start), (extent))


/// Return true if the environment variable with the given name is set
/// to a string that is not "0"

static UNUSED
bool env_is_true(cstr varname) {
    cstr val = getenv(varname);
    if (val) {
        if (val[0] == '\0') {
            return false;
        }
        if ((val[0] == '0') && (val[1] == '\0')) {
            return false;
        }
        return true;
    } else {
        return false;
    }
}


/// `MAIN` takes the name of the function to run when the program
/// starts. `mainfunction` receives a `slice` of `cstr` values which
/// are holding the program name in position 0 (usually, but not
/// actually guaranteed) and then the program arguments, and must
/// return a Result. If it returns an Err, the program will exit with
/// exit code 1, if it returns Ok, with exit code 0. (The Ok value
/// itself is currently being ignored.)

/// `MAIN` defines the function `main` (hence `mainfunction` cannot be
/// called `main`).

/// ```C
/// Result(Unit, UnicodeError) run(slice(cstr) argv) {
///     ...
/// }
/// 
/// MAIN(run);
/// ```

#define MAIN(mainfunction)                                              \
    int main(int argc, const char**argv) {                              \
        __CJ50_Mutex_debug = env_is_true("CJ50_DEBUG");                 \
        if_let_Ok(UNUSED _, (mainfunction)(new_slice_cstr(argv, argc))) { \
            return 0;                                                   \
        } else_Err(e) {                                                 \
            fprintln(stderr, &e);                                       \
            drop(e);                                                    \
            return 1;                                                    \
        } end_let_Ok;                                                   \
    }


#undef GET_THING

#include "cj50/plot.h"
