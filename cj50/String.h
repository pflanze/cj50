#pragma once

#include <cj50/gen/Option.h>
#include <cj50/CStr.h>
#include <cj50/char.h>
#include <cj50/instantiations/Vec_char.h>
#include <cj50/resret.h>


/// `String` is an owned, mutable type that holds a string of
/// characters, more precisely, an array of bytes, that represents a
/// text in UTF-8 encoding. Unlike C strings (see `CStr`), it does not
/// use a '\0' terminator, and can represent embedded '\0'
/// characters. It automatically resizes itself as needed to accept
/// additional text that is added.

/// It is implemented via a `Vec(char)`.

typedef struct String {
    Vec(char) vec;
} String;

static UNUSED
void drop_String(String s) {
    drop_Vec_char(s.vec);
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
int print_String(const String *s) {
    size_t len = s->vec.len;
    size_t res = fwrite(s->vec.ptr, 1, len, stdout);
    if (res < len) {
        return -1;
    }
    return res;
}

static UNUSED
int print_move_String(String s) {
    int res = print_String(&s);
    drop_String(s);
    return res;
}

static UNUSED
int print_debug_String(const String *s) {
    INIT_RESRET;
    size_t len = s->vec.len;
    char *ptr = s->vec.ptr;
    RESRET(putchar('"'));
    for (size_t i = 0; i < len; i++) {
        RESRET(_print_debug_char(ptr[i]));
    }
    RESRET(putchar('"'));
cleanup:
    return ret;
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

/// Create a String from a CStr, consuming the latter.

static UNUSED
String new_String_from_CStr(CStr s) {
    // XX todo: UTF-8 verification. (Or do that for CStr already?)
    size_t len = strlen(s.cstr);
    return (String) {
        .vec = (Vec(char)) {
            .ptr = s.cstr,
            .cap = len + 1,
            .len = len
        }       
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
size_t len_String(const String *s) {
    return s->vec.len;
}


/// Appends the given char to the end of this String.
static UNUSED
void push_String(String *s, char c) {
    push_Vec_char(&s->vec, c);
}


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



GENERATE_Option(String);

