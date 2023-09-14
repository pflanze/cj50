#include <cj50.h>

Result(Unit, String) run(slice(cstr) argv) {
    BEGIN_Result(Unit, String);

    let_Some_else(nitems_str, get(&argv, 1))
        RETURN_Err("Missing program argument: nitems", cleanup0);
    int nitems = TRY(parse_nat(*nitems_str), cleanup0);

    let_Some_else(niter_str, get(&argv, 2))
        RETURN_Err("Missing program argument: niter", cleanup0);
    int niter = TRY(parse_nat(*niter_str), cleanup0);

    let_Some_else(nsliceitems_str, get(&argv, 3))
        RETURN_Err("Missing program argument: nsliceitems", cleanup0);
    int nsliceitems = TRY(parse_nat(*nsliceitems_str), cleanup0);

    i64 range_base = nitems - nsliceitems;
    if (range_base < 0)
        RETURN_Err("Asked for a larger slice than the vector", cleanup0);


    AUTO v = with_capacity_Vec_int(nitems);

    if (true) {
        // Unsafe, but about 40/25=1.6x faster
        for (int i = 0; i < nitems; i++) {
            v.ptr[i] = i;
        }
        v.len = nitems;
    } else {
        for (int i = 0; i < nitems; i++) {
            push(&v, i);
        }
    }

    println(len(&v));

    i64 total = 0;
    for (int i = 0; i < niter; i++) {
        AUTO sl = slice_of(&v, range(range_base,
                                     range_base + nsliceitems));
        total += *at(&sl, MIN(i, nsliceitems - 1));
    }

    println(total);

    RETURN_Ok(Unit(), cleanup1);
cleanup1:
    drop(v);
cleanup0:
    END_Result();
}

MAIN(run);
