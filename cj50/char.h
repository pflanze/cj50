#pragma once

#include <cj50/gen/Vec.h>
#include <cj50/resret.h>


static UNUSED
void drop_char(const char UNUSED s) { }

static UNUSED
bool equal_char(const char *a, const char *b) {
    return *a == *b;
}

static UNUSED
bool equal_move_char(const char a, const char b) {
    return a == b;
}


static UNUSED
int _print_debug_char(char c) {
    if (c == '"') {
        return printf("\\\"");
    } else if (c == '\\') {
        return printf("\\\\");
    } else if (c == '\0') {
        return printf("\\0");
    } else if (c == '\n') {
        return printf("\\n");
    } else if (c == '\r') {
        return printf("\\r");
    } else if (c == '\t') {
        return printf("\\t");
    } else if (c == '\v') {
        return printf("\\v");
    } else if (c == '\f') {
        return printf("\\f");
    } else if (c == '\b') {
        return printf("\\b");
    } else if (c == '\a') {
        return printf("\\a");
    } else if (iscntrl(c)) {
        return printf("\\%03o", c);
    } else {
        return fputc(c, stdout);
    }
}

static UNUSED
int print_debug_char(const char *c) {
    INIT_RESRET;
    RESRET(fputc('\'', stdout));
    RESRET(_print_debug_char(*c));
    RESRET(fputc('\'', stdout));
cleanup:
    return ret;
}


GENERATE_Option(char);

