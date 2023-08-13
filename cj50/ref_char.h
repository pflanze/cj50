#pragma once

#include <cj50/resret.h>
#include <cj50/gen/Option.h>
#include <cj50/gen/ref.h>


GENERATE_ref(char);


static UNUSED
void drop_ref_char(UNUSED const ref(char) s) { }

static UNUSED
bool equal_ref_char(const ref(char) *a, const ref(char) *b) {
    return **a == **b;
}

static UNUSED
bool equal_move_ref_char(const ref(char) a, const ref(char) b) {
    return *a == *b;
}


static UNUSED
int print_debug_ref_char(const ref(char) *c) {
    INIT_RESRET;
    RESRET(print_move_cstr("&\'"));
    RESRET(_print_debug_char(**c));
    RESRET(fputc('\'', stdout));
cleanup:
    return ret;
}


GENERATE_Option(ref(char));
