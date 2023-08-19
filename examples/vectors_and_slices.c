#include <cj50.h>

int main(int argc, char** argv) {
    char **numbers = argv + 1;
    int numbers_len = argc - 1;
    if (numbers_len < 1) {
        printf("Usage: N=10 %s 2 3 7 10 13 99\n", argv[0]);
        return 1;
    }

    int n = unwrap(parse_int(getenv("N")));
    Vec(int) parsed_numbers = new_Vec_int();
    for (int i = 0; i < numbers_len; i++) {
        push(&parsed_numbers, unwrap(parse_int(numbers[i])));
    }

    DBG(&parsed_numbers);
    slice(int) sl = slice_of(&parsed_numbers, range(2, 6));
    //                ^ this will abort if parsed_numbers has fewer than 6 elements
    DBG(&sl);
    DBG(len(&sl));

    slice(int) sl2 = slice_of(&sl, range(1, 3));
    DBG(sl2); // slices are already borrowed types and Copy, so don't
              // really need &
    DBG(len(&sl2)); // except that a move variant is currently only
                    // implemented for print_debug (and hence DBG),
                    // not len.

    DBG(at(&sl2, 1));
    DBG(*at(&sl2, 1) == n);

    set(&parsed_numbers, 4, 123);
    // That was evil: *should not* mutate the Vec while slices are
    // still active, because they see the change:
    DBG(sl); // but should NOT use that slice anymore after a mutating operation!!
    DBG(sl2); // but should NOT use that slice anymore after a mutating operation!!

    mutslice(int) msl = mutslice_of(&parsed_numbers, range(2, 6));
    slice(int) sl2b = slice_of(&msl, range(1, 3));
    DBG(sl2b); // but should NOT use msl after getting an immutable slice from now on!
    mutslice(int) msl2 = mutslice_of(&msl, range(1, 3));

    set(&msl2, 0, 456);
    DBG(msl2);
    DBG(sl); // but should NOT use that slice anymore after a mutating operation!!

    DBG(&parsed_numbers);

    drop(parsed_numbers);
}
