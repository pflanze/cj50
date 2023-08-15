#include <cj50.h>

// Example using String as the error type as suggested by the
// `newcj50` script.

Result(Unit, String) run(slice(cstr) argv) {
    BEGIN_Result(Unit, String);
    if (argv.len > 2) {
        String e = new_String_from("Too many arguments: expected 1 or less, got ");
        append_move(&e, new_String_from(argv.len - 1));
        RETURN_Err(e, cleanup0);
    } else {
        RETURN_Err("Error anyway", cleanup0);
    }
cleanup0:
    END_Result();
}

MAIN(run);
