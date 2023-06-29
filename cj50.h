#ifndef CJ50_H_
#define CJ50_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>

#include "cjmath.h"
#include "sdlutil.h"
#include "u8l/basic-util.h"
#include "u8l/Option.h"



void _print_debug_char(char c) {
    if (c == '"') {
        printf("\\\"");
    } else if (c == '\\') {
        printf("\\\\");
    } else if (c == '\0') {
        printf("\\0");
    } else if (c == '\n') {
        printf("\\n");
    } else if (c == '\r') {
        printf("\\r");
    } else if (c == '\t') {
        printf("\\t");
    } else if (c == '\v') {
        printf("\\v");
    } else if (c == '\f') {
        printf("\\f");
    } else if (c == '\b') {
        printf("\\b");
    } else if (c == '\a') {
        printf("\\a");
    } else if (iscntrl(c)) {
        printf("\\%03o", c);
    } else {
        fputc(c, stdout);
    }
}

void print_debug_char(char c) {
    fputc('\'', stdout);
    _print_debug_char(c);
    fputc('\'', stdout);
}


typedef char* string;

void drop_string(const string s) {
    free(s);
}

// receive pointers to pointers just for standard in Option
bool equal_string(const string *a, const string *b) {
    return strcmp(*a, *b) == 0;
}


DEFTYPE_Option(string);

void print_string(const char* str) {
    printf("%s", str);
}

void print_debug_string(const char* str) {
    print_string("\"");
    while (*str) {
        _print_debug_char(*str);
        str++;
    }
    print_string("\"");
}

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
                return none_string();
            } else {
                DIE_("Could not get a line from stdin: %s",
                     strerror(errno));
            }
        }
        size_t l = strlen(line);
        // Always must have either a '\n' at the end or some other
        // text, since the EOF case is handled above.
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


static UNUSED
bool equal_int(const int *a, const int *b) {
    return *a == *b;
}

static UNUSED
void drop_int(const int UNUSED a) {}

DEFTYPE_Option(int);

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
            if (*tail == '\0') {
                if (n >= INT_MIN && n <= INT_MAX) {
                    drop_Option_string(s);
                    return some_int(n);
                } else {
                    print_string("Your answer is not within the range of "
                                 "numbers of the `int` type.");
                }
            } else {
                print_string("Please enter only a number with nothing "
                             "after it.");
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

void print_int(int n) {
    printf("%i", n);
}

// Don't use uint willy-nilly, UBSan will not catch overflows!
typedef unsigned int uint;

void print_uint(uint n) {
    printf("%u", n);
}


typedef int nat;

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

void print_nat(int n) {
    if (n > 0) {
        printf("%i", n);
    } else {
        DIE_("error: print_nat(%i): argument out of range", n);
    }
}


typedef int nat0;

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

void print_nat0(int n) {
    if (n >= 0) {
        printf("%i", n);
    } else {
        DIE_("error: print_nat0(%i): argument out of range", n);
    }
}


static UNUSED
void drop_float(float UNUSED x) { }

static UNUSED
bool equal_float(const float *a, const float *b) {
    return *a == *b;
}

DEFTYPE_Option(float);

void print_float(float x) {
    printf("%g", x);
}

// largely copy-paste of get_int
Option(float) get_float() {
    while (true) {
        Option(string) s = get_string();
        if (!s.is_some) {
            return none_float();
        }
        char *tail;
        errno = 0;
        float x = strtof(s.value, &tail);
        if (errno == 0) {
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

char* new_chars(size_t len) {
    return new_array_of_type_and_len("char", sizeof(char), len);
}
string new_string(size_t len) {
    return new_array_of_type_and_len("char", sizeof(char), len);
}

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

int* new_ints(size_t len) {
    return new_array_of_type_and_len("int", sizeof(int), len);
}
nat* new_nats(size_t len) {
    return new_array_of_type_and_len("nat", sizeof(nat), len);
}
nat0* new_nat0s(size_t len) {
    return new_array_of_type_and_len("nat0", sizeof(nat0), len);
}

float* new_floats(size_t len) {
    return new_array_of_type_and_len("float", sizeof(float), len);
}

/*
  does this work?
  #define INITIALIZE_ARRAY(ary, members) *ary = members
  */

#define PRINT_ARRAY(print_typ, ary, len)        \
    print_string("[");                          \
    for (size_t i = 0; i < len; i++) {          \
        if (i > 0) {                            \
            print_string(", ");                 \
        }                                       \
        print_typ(ary[i]);                      \
    }                                           \
    print_string("]");

void print_debug_chars(const char* ary, size_t len) {
    PRINT_ARRAY(print_debug_char, ary, len);
}

void print_debug_strings(const string* ary, size_t len) {
    PRINT_ARRAY(print_debug_string, ary, len);
}

void print_debug_ints(const int* ary, size_t len) {
    PRINT_ARRAY(print_int, ary, len);
}
void print_debug_nats(const nat* ary, size_t len) {
    PRINT_ARRAY(print_nat, ary, len);
}
void print_debug_nat0s(const nat0* ary, size_t len) {
    PRINT_ARRAY(print_nat0, ary, len);
}

void print_debug_floats(const float* ary, size_t len) {
    PRINT_ARRAY(print_float, ary, len);
}

#undef PRINT_ARRAY


// These bypass runtime checks from restricted number types like nats!

#define print(v)                                \
    _Generic((v)                                \
             , char*: print_string              \
             , char: putchar                    \
             , int: print_int                   \
             , uint: print_uint                 \
             , float: print_float               \
        )(v)

#define print_debug(v)                          \
    _Generic((v)                                \
             , char*: print_debug_string        \
             , char: print_debug_char           \
             , int: print_int                   \
             , uint: print_uint                 \
             , float: print_float               \
             , Vec2: print_debug_Vec2           \
             , Vec3: print_debug_Vec3           \
        )(v)

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


#define resize(var, oldlen, newlen)             \
    _Generic((var)                              \
             , string*: resize_strings          \
             , int*: resize_ints                \
             , float*: resize_floats            \
        )((var), (oldlen), (newlen))


#define drop(v)                                 \
    _Generic((v)                                \
             , char*: drop_string               \
             , int*: free                       \
             , float*: free                     \
             , Vec2*: free                    \
             , Vec3*: free                    \
        )(v)

#define drop_array(v, len)                      \
    _Generic((v)                                \
             , string*: free_strings            \
        )((v), (len))


#define D(v)                                    \
    do {                                        \
        print("D(" #v ") => ");                 \
        print_debug(v);                         \
        print("\n");                            \
    } while (0)

#define DA(v, len)                              \
    do {                                        \
        print("DA(" #v ", " #len ") => ");      \
        print_debug_array(v, len);              \
        print("\n");                            \
    } while (0)


#endif /* CJ50_H_ */
