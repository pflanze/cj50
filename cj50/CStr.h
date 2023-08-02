#pragma once

#include <cj50/gen/Option.h>
#include <cj50/char.h>


#define RESRET(e)                \
    res = (e);                   \
    if (res < 0) { return res; } \
    ret += res;


/// `cstr` is a non-mutable borrowed type that represents a "C
/// cstr". It is an array of `char`, that represents the text, and a
/// '\0' character after it to signal the end. There is no length
/// information, the length has to be determined by walking the array
/// until encountering the '\0' character (`strlen` will do that).
typedef const char* cstr;

// even though not owned, we need this because Option refers to it
static UNUSED
void drop_cstr(UNUSED cstr s) {}

// receive pointers to pointers just for standard in Option
static UNUSED
bool equal_cstr(const cstr *a, const cstr *b) {
    return strcmp(*a, *b) == 0;
}

static UNUSED
bool equal_move_cstr(cstr a, cstr b) {
    return strcmp(a, b) == 0;
}


int print_cstr(cstr str) {
    return printf("%s", str);
}

static
int print_debug_cstr(const cstr *s) {
    int ret = 0;
    int res;
    RESRET(print_cstr("\""));

    cstr str = *s;
    while (*str) {
        RESRET(_print_debug_char(*str));
        str++;
    }
    RESRET(print_cstr("\""));
    return ret;
}

#undef RESRET


GENERATE_Option(cstr);

