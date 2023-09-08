#include <cj50.h>


Result(Unit, String) run(UNUSED slice(cstr) argv) {
    BEGIN_Result(Unit, String);

    // DBGV can and must be used for non-reference values that you want to pick
    // up from DBGV's return:
    String s = DBGV(String(at(&argv, 0)));
    append_move(&s, DBGV(String(argv.len)));
    DBG(&s);  // Reference is fine, means, "do not consume".
    // DBGV(&s); // Reference is *not* OK for DBGV (see its docs).
    DBG(s); // *Not* DBGV, if we do not want to use the return value; drops the value.

    RETURN_Ok(Unit(), cleanup);
cleanup:
    END_Result();
}

MAIN(run);
