#ifndef CJ50_STRING_H_
#define CJ50_STRING_H_

#include "Option.h"
#include "cj50/char.h"


#define RESRET(e)                \
    res = (e);                   \
    if (res < 0) { return res; } \
    ret += res;


/// `string` is an array of `char`s. It always must have the special
/// '\0' char in it somewhere, after the end of the text that is
/// stored in the string, to signal the end of the text.
typedef char* string;

static UNUSED
void drop_string(const string s) {
    free(s);
}

// receive pointers to pointers just for standard in Option
static UNUSED
bool equal_string(const string *a, const string *b) {
    return strcmp(*a, *b) == 0;
}

int print_string(const char* str) {
    return printf("%s", str);
}

static UNUSED
int print_debug_string(const char* str) {
    int ret = 0;
    int res;
    RESRET(print_string("\""));
    
    while (*str) {
        RESRET(_print_debug_char(*str));
        str++;
    }
    RESRET(print_string("\""));
    return ret;
}

#undef RESRET


GENERATE_Option(string);

#endif /* CJ50_STRING_H_ */
