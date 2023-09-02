#pragma once

#include <cj50/gen/Option.h>
#include <cj50/CStr.h>
#include <cj50/char.h>
#include <cj50/instantiations/Vec_char.h>
#include <cj50/resret.h>


GENERATE_Option(slice(char));


/// `String` is an owned, mutable type that holds a string of
/// characters, more precisely, an array of bytes, that represents a
/// text in UTF-8 encoding. Unlike C strings (see `CStr`), it does not
/// use a '\0' terminator, and can represent embedded '\0'
/// characters. It automatically resizes itself as needed to accept
/// additional text that is added.

/// It is implemented as a wrapper around `Vec(char)`. It adds the
/// guarantee that the contents are in correct UTF-8 encoding.

typedef struct String {
    Vec(char) vec;
} String;

/// `strslice` is a borrowed, immutable type that, just like `String`,
/// holds a string of characters, more precisely, an array of bytes,
/// that represents a text in UTF-8 encoding.

/// It is implemented as a wrapper around `slice(char)`. It adds the
/// guarantee that the contents are in correct UTF-8 encoding.

typedef struct strslice {
    slice(char) slice;
} strslice;

#define new_strslice(ptr, len)                      \
    ((strslice) {                                   \
        .slice = new_slice_char((ptr), (len))       \
     })


static UNUSED
void drop_String(String s) {
    drop_Vec_char(s.vec);
}

static UNUSED
void drop_strslice(UNUSED strslice s) {}


/// Get the string slice of the whole String.
static UNUSED
strslice deref_String(const String *s) {
    return new_strslice(s->vec.ptr, s->vec.len);
}


/// Clear the string, removing all characters.

/// Note that this method has no effect on the allocated capacity of
/// the String.

static UNUSED
void clear_String(String *s) {
    clear_Vec_char(&s->vec);
}

static UNUSED
bool equal_String(const String *a, const String *b) {
    return equal_Vec_char(&a->vec, &b->vec);
}

static UNUSED
bool equal_strslice(const strslice *a, const strslice *b) {
    return equal_slice_char(&a->slice, &b->slice);
}

static UNUSED
int print_strslice(const strslice *s) {
    size_t len = s->slice.len;
    size_t res = fwrite(s->slice.ptr, 1, len, stdout);
    if (res < len) {
        return -1;
    }
    return res;
}

static UNUSED
int print_move_strslice(strslice s) {
    return print_strslice(&s);
}

static UNUSED
int print_String(const String *s) {
    return print_move_strslice(deref_String(s));
}

static UNUSED
int print_move_String(String s) {
    int res = print_String(&s);
    drop_String(s);
    return res;
}

static UNUSED
int fprintln_strslice(FILE *out, const strslice *s) {
    INIT_RESRET;
    size_t len = s->slice.len;
    size_t r = fwrite(s->slice.ptr, 1, len, out);
    if (r < len) {
        return -1;
    }
    ret = r; // XX safety
    RESRET(fputc('\n', out));
cleanup:
    return ret;
}

static UNUSED
int fprintln_move_strslice(FILE *out, strslice s) {
    return fprintln_strslice(out, &s);
}

static UNUSED
int fprintln_String(FILE *out, const String *s) {
    return fprintln_move_strslice(out, deref_String(s));
}

static UNUSED
int print_debug_strslice(const strslice *s) {
    INIT_RESRET;
    size_t len = s->slice.len;
    const char *ptr = s->slice.ptr;
    RESRET(putchar('"'));
    for (size_t i = 0; i < len; i++) {
        RESRET(_print_debug_char(ptr[i]));
    }
    RESRET(putchar('"'));
cleanup:
    return ret;
}

static UNUSED
int print_debug_move_strslice(strslice s) {
    return print_debug_strslice(&s);
}

static UNUSED
int print_debug_String(const String *s) {
    return print_debug_move_strslice(deref_String(s));
}

static UNUSED
int print_debug_move_String(String s) {
    int r = print_debug_String(&s);
    drop_String(s);
    return r;
}


/// Create a new empty String.

static UNUSED
String new_String() {
    return (String) {
        .vec = new_Vec_char()       
    };
}

/// Create a String with the given `capacity` (but 0 current length).
static UNUSED
String with_capacity_String(size_t capacity) {
    return (String) {
        .vec = with_capacity_Vec_char(capacity)
    };
}

/// The length in *bytes*, not characters. This operation is fast (has
/// a constant cost), unlike `strlen` for C strings.

static UNUSED
size_t len_strslice(const strslice *s) {
    return s->slice.len;
}

/// The length in *bytes*, not characters. This operation is fast (has
/// a constant cost), unlike `strlen` for C strings.

static UNUSED
size_t len_String(const String *s) {
    return s->vec.len;
}


/// Appends the given char to the end of this String.
static UNUSED
void push_String(String *s, char c) {
    push_Vec_char(&s->vec, c);
}


/// Appends the given String `b` to the end of String `a`, emptying
/// `b`.
static UNUSED
void append_String_String(String *a, String *b) {
    append_Vec_char(&a->vec, &b->vec);
}

/// Appends the given String `b` to the end of String `a`, consuming
/// `b`.
static UNUSED
void append_move_String_String(String *a, String b) {
    append_Vec_char(&a->vec, &b.vec);
    drop_String(b);
}


// Can't offer cstr_strslice except via copying (because of the need
// to add a '\0'), so don't (for now).


/// Get `s` as a C string, if possible--it's only possible if there
/// are no embedded `'\0'` characters.

/// The returned `cstr` is borrowed and shares storage with `s`, so
/// `s` may not be mutated while the `cstr` is in use.
static UNUSED
Option(cstr) cstr_String(String *s) {
    size_t cap = s->vec.cap;
    size_t len = s->vec.len;
    // Do we have embedded `'\0'`s?
    if (memchr(s->vec.ptr, 0, len)) {
        return none_cstr();
    }
    // Make sure we have a `'\0'` terminator
    if (!(cap > len)) {
        reserve_Vec_char(&s->vec, len);
    }
    char *ptr = s->vec.ptr; // get fresh, after reserve_Vec_char!
    ptr[len] = '\0';
    return some_cstr(ptr);
}


GENERATE_Option(strslice);


/// Get a slice of the string. Note that the given range of indices
/// must be in byte positions, not unicode codepoints. No check is
/// done that the given positions are at UTF-8 boundaries. Aborts for
/// invalid indices. (Only use if sure about the indices, otherwise
/// use `get_slice_of_String`.)

static UNUSED
strslice unsafe_slice_of_String(const String *s, Range range) {
    return (strslice) {
        .slice = slice_of_Vec_char(&s->vec, range)
    };
}


#define T int
#define FORMATSTRING "%i"
#include <cj50/gen/template/new_String_from.h>
#undef FORMATSTRING
#undef T

#define T size_t
#define FORMATSTRING "%li"
#include <cj50/gen/template/new_String_from.h>
#undef FORMATSTRING
#undef T


// for new_from
static UNUSED
String new_String_from_String(String s) {
    return s;
}

GENERATE_Option(String);

