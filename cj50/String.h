#pragma once

#include <cj50/gen/Option.h>
#include <cj50/string.h>
// #include <cj50/char.h>

/// `String` is an owned type that holds a "C string", an string of
/// characters, more precisely, an array of `char`, that represents
/// the text, and a '\0' character after it to signal the end. There
/// is no length information, the length has to be determined by
/// walking the array until encountering the '\0' character (`strlen`
/// will do that on the contained array, or `len` on the String
/// wrapper type).

typedef struct String {
    char* str;
} String;

#define String(x) ((String) { .str = (x) })

static UNUSED
void drop_String(String s) {
    free(s.str);
}

static UNUSED
bool equal_String(const String *a, const String *b) {
    return strcmp(a->str, b->str) == 0;
}

// XX change to borrow? But do it for all of `print`.
int print_String(const String s) {
    return print_string(s.str);
}

static UNUSED
int print_debug_String(const String *s) {
    // Have to cast here because C only silences non-const to const
    // casting if `const` is on the left-most (outmost) level:
    return print_debug_string((const string *)&s->str);
}

static UNUSED
int print_debug_move_String(String s) {
    int r = print_debug_String(&s);
    drop_String(s);
    return r;
}


GENERATE_Option(String);

