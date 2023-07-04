//! This provides a collection of various useful utilities. It
//! includes cjmath.h, sdlutil.h, and Option.h

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
// #include "cj50/plot.h"  include it at the end to pass D
#include "cj50/string.h"
#include "cj50/int.h"
#include "cj50/float.h"


#define RESRET(e)                \
    res = (e);                   \
    if (res < 0) { return res; } \
    ret += res;


/// Read a string from standard input, terminated by a
/// newline. Returns none on end of file (when ctl-d is pressed).
Option(string) get_string() {
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


/// Read an integer number from standard input, terminated by a
/// newline. Returns none on end of file (when ctl-d is pressed).
Option(int) get_int() {
    while (true) {
        Option(string) s = get_string();
        if (!s.is_some) {
            return none_int();
        }
        char *tail;
        errno = 0;
        long n = strtol(s.value, &tail, 10);
        if (errno == 0) {
            while (*tail == ' ') {
                tail++;
            }
            if (*tail == '\0') {
                if (n >= INT_MIN && n <= INT_MAX) {
                    drop_Option_string(s);
                    return some_int(n);
                } else {
                    print_string("Your answer is not within the range of "
                                 "numbers of the `int` type.");
                }
            } else {
                print_string("Your answer has invalid text after the number.");
            }
        } else {
            printf("Your answer is not an integer number in range possible "
                   "for the `long` type: %s.",
                   strerror(errno));
        }
        drop_Option_string(s);
        print_string(" Please enter an integer number: ");
    }
}

int print_int(int n) {
    return printf("%i", n);
}

// Don't use uint willy-nilly, UBSan will not catch overflows!
typedef unsigned int uint;

int print_uint(uint n) {
    return printf("%u", n);
}


/// Read a natural number from standard input, terminated by a
/// newline. Returns none on end of file (when ctl-d is pressed).
Option(int) get_nat() {
    while (true) {
        Option(int) i = get_int();
        if (!i.is_some) {
            return none_int();
        }
        if (i.value > 0) {
            return i;
        }
        print_string("Your answer is not above zero.");
        print_string(" Please enter a natural number: ");
    }
}

int print_nat(int n) {
    if (n > 0) {
        return printf("%i", n);
    } else {
        DIE_("error: print_nat(%i): argument out of range", n);
    }
}


/// Read a natural number or zero from standard input, terminated by a
/// newline. Returns none on end of file (when ctl-d is pressed).
Option(int) get_nat0() {
    while (true) {
        Option(int) i = get_int();
        if (!i.is_some) {
            return none_int();
        }
        if (i.value >= 0) {
            return i;
        }
        print_string("Your answer is negative.");
        print_string(" Please enter a natural number or zero: ");
    }
}

int print_nat0(int n) {
    if (n >= 0) {
        return printf("%i", n);
    } else {
        DIE_("error: print_nat0(%i): argument out of range", n);
    }
}


int print_float(float x) {
    return printf("%g", x);
}

/// Read a floating point number or zero from standard input,
/// terminated by a newline. Returns none on end of file (when ctl-d is
/// pressed).
Option(float) get_float() {
    // largely copy-paste of get_int
    while (true) {
        Option(string) s = get_string();
        if (!s.is_some) {
            return none_float();
        }
        char *tail;
        errno = 0;
        float x = strtof(s.value, &tail);
        if (errno == 0) {
            while (*tail == ' ') {
                tail++;
            }
            if (*tail == '\0') {
                drop_Option_string(s);
                return some_float(x);
            } else {
                print_string("Please enter only a number with nothing "
                             "after it.");
            }
        } else {
            printf("Your answer is not a floating point number in the "
                   "possible range for the `float` type: %s.",
                   strerror(errno));
        }
        drop_Option_string(s);
        print_string(" Please enter a floating point number: ");
    }
}


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

/// Allocate and return a new array of `len` char values. Aborts
/// when there is not enough memory (never returns the NULL
/// pointer). The slots in the returned array are all initialized to
/// the '\0' char. This is the same as `new_string`.
char* new_chars(size_t len) {
    return new_array_of_type_and_len("char", sizeof(char), len);
}

/// Allocate and return a new string of capacity `len`. Aborts when
/// there is not enough memory (never returns the NULL pointer). The
/// string is set to the empty string. This is the same as
/// `new_chars`.
string new_string(size_t len) {
    return new_array_of_type_and_len("char", sizeof(char), len);
}

/// Allocate and return a new array of `len` string values. Aborts
/// when there is not enough memory (never returns the NULL
/// pointer). The slots in the returned array are all initialized to
/// the NULL pointer.
string* new_strings(size_t len) {
    return new_array_of_type_and_len("string", sizeof(string), len);
}

void free_strings_slice(string* ary, size_t len) {
    for (size_t i = 0; i < len; i++) {
        string s = ary[i];
        if (s) {
            free(s);
        }
    }
}

void free_strings(string* ary, size_t len) {
    free_strings_slice(ary, len);
    free(ary);
}


/// Allocate and return a new array of `len` int values. Aborts when
/// there is not enough memory (never returns the NULL pointer). The
/// slots in the returned array are all initialized to the value 0.
int* new_ints(size_t len) {
    return new_array_of_type_and_len("int", sizeof(int), len);
}

/// Allocate and return a new array of `len` float values. Aborts when
/// there is not enough memory (never returns the NULL pointer). The
/// slots in the returned array are all initialized to the value 0.
float* new_floats(size_t len) {
    return new_array_of_type_and_len("float", sizeof(float), len);
}

/*
  does this work?
  #define INITIALIZE_ARRAY(ary, members) *ary = members
  */

#define PRINT_ARRAY(print_typ, ary, len)        \
    int ret = 0;                                \
    int res;                                    \
    RESRET(print_string("["));                  \
    for (size_t i = 0; i < len; i++) {          \
        if (i > 0) {                            \
            RESRET(print_string(", "));         \
        }                                       \
        RESRET(print_typ(ary[i]));              \
    }                                           \
    RESRET(print_string("]"));                  \
    return ret;

int print_debug_chars(const char* ary, size_t len) {
    PRINT_ARRAY(print_debug_char, ary, len);
}

int print_debug_strings(const string* ary, size_t len) {
    PRINT_ARRAY(print_debug_string, ary, len);
}

int print_debug_ints(const int* ary, size_t len) {
    PRINT_ARRAY(print_int, ary, len);
}

int print_debug_floats(const float* ary, size_t len) {
    PRINT_ARRAY(print_float, ary, len);
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
             , char*: print_string              \
             , char: putchar                    \
             , int: print_int                   \
             , uint: print_uint                 \
             , float: print_float               \
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
             , char*: print_debug_string                      \
             , char: print_debug_char                         \
             , int: print_int                                 \
             , uint: print_uint                               \
             , float: print_float                             \
             , Vec2: print_debug_Vec2                         \
             , Vec3: print_debug_Vec3                         \
             , Option(string): move_print_debug_Option_string \
             , Option(int): move_print_debug_Option_int       \
             , Option(float): move_print_debug_Option_float   \
             , Option(string)*: print_debug_Option_string     \
             , Option(int)*: print_debug_Option_int           \
             , Option(float)*: print_debug_Option_float       \
             , Line2: print_debug_Line2                       \
             , Rect2: print_debug_Rect2                       \
        )(v)


/// Prints the given array in a programmer's view, for debugging
/// purposes.

/// Returns an `int` that when negative signifies an error (check
/// `errno` for the error in this case), and when positive signifies
/// the number of bytes written.

/// See `print_debug` for printing non-array values.

#define print_debug_array(v, len)               \
    _Generic((v)                                \
             , char*: print_debug_chars         \
             , string*: print_debug_strings     \
             , int*: print_debug_ints           \
             , float*: print_debug_floats       \
        )((v), (len))


#define RESIZE_ARRAY_test_equal(ary, oldlen, newlen)    \
    if (newlen == oldlen) {                             \
        return ary;                                     \
    }
#define RESIZE_ARRAY_free(ary, oldlen, newlen)          \
    if (newlen < oldlen) {                              \
        for (size_t i = newlen; i < oldlen; i++) {      \
            if (ary[i]) { free(ary[i]); }               \
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


string* resize_strings(string* ary, size_t oldlen, size_t newlen) {
    RESIZE_ARRAY_test_equal(ary, oldlen, newlen);
    RESIZE_ARRAY_free(ary, oldlen, newlen);
    RESIZE_ARRAY_realloc_and_fill(ary, oldlen, newlen, string, NULL);
}

int* resize_ints(int* ary, size_t oldlen, size_t newlen) {
    RESIZE_ARRAY_test_equal(ary, oldlen, newlen);
    RESIZE_ARRAY_realloc_and_fill(ary, oldlen, newlen, int, 0);
}

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
#define resize(v, oldlen, newlen)               \
    _Generic((v)                                \
             , string*: resize_strings          \
             , int*: resize_ints                \
             , float*: resize_floats            \
        )((v), (oldlen), (newlen))


/// Frees the resources held by the given value (including resources
/// held by contained values, recursively, unlike the `free` function).
#define drop(v)                                        \
    _Generic((v)                                       \
             , char*: drop_string                      \
             , int*: free                              \
             , float*: free                            \
             , Vec2*: free                             \
             , Vec3*: free                             \
             , Option(string): drop_Option_string      \
             , Option(int): drop_Option_int            \
             , Option(float): drop_Option_float        \
        )(v)

/// Frees the resources held by the given array,
/// including all the values contained in the array (including
/// resources held by contained values, recursively). For arrays
/// holding pointers, array slots containing the NULL pointer are
/// skipped.
#define drop_array(v, len)                      \
    _Generic((v)                                \
             , string*: free_strings            \
        )((v), (len))


/// Takes two values of the same type and returns a `bool`. Returns
/// `true` if `a` and `b` are structurally equivalent.
#define equal(a, b)                                     \
    _Generic((v)                                        \
             , Option(string): equal_Option_string      \
             , Option(int): equal_Option_int            \
             , Option(float): equal_Option_float        \
        )((a), (b))

/// Takes a value of some type `T` and returns a `some` variant of
/// `Option(T)` containing the value.
#define some(v)                                         \
    _Generic((v)                                        \
             , string: some_string                      \
             , int: some_int                            \
             , float: some_float                        \
        )(v)

/// Returns the none variant of `Option(T)` for the given
/// T. Equivalent to `none_T()` using the type name of T, but
/// `none(T)` allows T to also be a `typeof` expression.
#define none(T)                                         \
    _Generic(*((T*)(NULL))                              \
             , string: none_string                      \
             , int: none_int                            \
             , float: none_float                        \
        )()

/// Takes an `Option(T)` for some type `T` and returns the contained
/// value of type `T` if it is a `some`, but aborts if it is a `none`.

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

#define unwrap(v)                                       \
    _Generic((v)                                        \
             , Option(string): unwrap_Option_string     \
             , Option(int): unwrap_Option_int           \
             , Option(float): unwrap_Option_float       \
        )(v)


/// `D`ebug: print the expression `expr` and the value it evaluated
/// to, for debugging purposes (calls `print_debug` on the value).
#define D(expr)                                 \
    do {                                        \
        print("DEBUG: " #expr " == ");          \
        print_debug(expr);                      \
        print("\n");                            \
    } while (0)

/// `D`ebug `A`rray: print the expression `expr` and the array value
/// it evaluated to, for debugging purposes (calls
/// `print_debug_array`). `len` must give the length of the array.
#define DA(expr, len)                                   \
    do {                                                \
        print("DEBUG: " #expr ", " #len " == ");        \
        print_debug_array(expr, len);                   \
        print("\n");                                    \
    } while (0)


#undef RESRET

#include "cj50/plot.h"
