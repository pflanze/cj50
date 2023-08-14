#include <cj50.h>
#include <cj50/instantiations/Result_Unit__UnicodeError.h>

void usage(cstr arg0, cstr msg) {
    fprintf(stderr, "error: %s\n", msg);
    fprintf(stderr, "usage: %s 'somestring'\n", arg0);
    fprintf(stderr, "  decodes the argument and shows in various views.\n");
    exit(1);
}

Result(Unit, UnicodeError) run(slice(cstr) argv) {
    BEGIN_Result(Unit, UnicodeError);

    if (argv.len != 2) {
        DBG(uchar("\n"));
        DBG(uchar("ä"));
        DBG(uchar("↓"));
        usage(argv.ptr[0], "need 1 argument");
    }

    cstr str = argv.ptr[1];

    CFile in = TRY(memopen_CFile((void*)str, strlen(str), "r"), cleanup1);
    Vec(ucodepoint) v = new_Vec_ucodepoint();
    while_let_Some(c, TRY(get_ucodepoint_unlocked(&in), cleanup2)) {
        push(&v, c);
    }
    DBG(v);

    AUTO v2 = TRY(new_Vec_utf8char_from_cstr(str), cleanup2);
    DBG(&v2);

    while_let_Some(c, pop(&v2)) {
        println(c);
    }

    RETURN_Ok(Unit(), cleanup3);
cleanup3:
    drop(v2);
cleanup2:
    drop(in);
cleanup1:
    END_Result();
}

MAIN(run);
