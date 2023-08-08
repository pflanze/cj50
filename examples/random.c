#include <cj50.h>

int main(int argc, cstr* argv) {
    if (argc != 2) {
        DIE("usage: random range");
    }
    int range;
    if (getenv("EXPLICIT")) {
        Result(int, ParseError) _range = parse_int(argv[1]);
        DBG(_range);
        if (! _range.is_ok) {
            CStr s = string_from_ParseError(&_range.err);
            DIE_("argument 1 %s", s.cstr);
            drop(s); // well :)
        }
        range = _range.ok;
    } else {
        range = unwrap(parse_int(argv[1]));
    }

    for (int i = 0; i < 10; i++) {
        int r = random_int(range);
        print(r);
        print("\n");
        assert(r >= 0);
        assert(r < range);
    }

    print("And some floats:\n");
    for (int i = 0; i < 10; i++) {
        __auto_type r = random_float();
        print(r);
        print("\n");
        assert(r >= 0.f);
        assert(r < 1.f);
    }

    print("And doubles:\n");
    for (int i = 0; i < 10; i++) {
        __auto_type r = random_double();
        print(r);
        print("\n");
        assert(r >= 0.);
        assert(r < 1.);
    }

}
