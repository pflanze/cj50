#pragma once

#include <cj50/gen/Option.h>
#include <cj50/char.h>


#define RESRET(e)                \
    res = (e);                   \
    if (res < 0) { return res; } \
    ret += res;


/// `string` is a non-mutable borrowed type that represents a "C
/// string". It is an array of `char`, that represents the text, and a
/// '\0' character after it to signal the end. There is no length
/// information, the length has to be determined by walking the array
/// until encountering the '\0' character (`strlen` will do that).
typedef const char* string;

// even though not owned, we need this because Option refers to it
static UNUSED
void drop_string(UNUSED string s) {}

// receive pointers to pointers just for standard in Option
static UNUSED
bool equal_string(const string *a, const string *b) {
    return strcmp(*a, *b) == 0;
}

static UNUSED
bool equal_move_string(string a, string b) {
    return strcmp(a, b) == 0;
}


int print_string(string str) {
    return printf("%s", str);
}

static
int print_debug_string(const string *s) {
    int ret = 0;
    int res;
    RESRET(print_string("\""));

    string str = *s;
    while (*str) {
        RESRET(_print_debug_char(*str));
        str++;
    }
    RESRET(print_string("\""));
    return ret;
}

#undef RESRET


GENERATE_Option(string);

