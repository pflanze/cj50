#include <cj50.h>

void usage(cstr arg0, cstr msg) {
    fprintf(stderr, "error: %s\n", msg);
    fprintf(stderr, "usage: %s 'somestring'\n", arg0);
    fprintf(stderr, "  decodes the argument and shows in various views.\n");
    exit(1);
}

GENERATE_Result(Unit, UnicodeError);

Result(Unit, UnicodeError) run(char *str) {
    BEGIN_Result(Unit, UnicodeError);

    LET_Ok(in, memopen_CFile(str, strlen(str)+1, "r"), cleanup1);

    Vec(ucodepoint) v = new_Vec_ucodepoint();

    while_let_Some(c, PROPAGATE_Result(get_ucodepoint_unlocked(&in),
                                       cleanup2)) {
        push(&v, c);
    }
    DBG(v);
    RETURN_Ok(Unit(), cleanup2);
    
cleanup2:
    drop(in);
cleanup1:
    END_Result();
}

int main(int argc, char** argv) {
    if (argc != 2)
        usage(argv[0], "need 1 argument");

    if_let_Ok(UNUSED _, run(argv[1])) {
        exit(0);
    } else_Err(e) {
        DBG(&e); //
        fprintln(stdout, &e);
        drop(e);
        exit(1);
    } end_let_Ok;
}
