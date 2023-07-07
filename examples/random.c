#include <cj50.h>

int main(int argc, string* argv) {
    if (argc != 2) {
        DIE("usage: random range");
    }
    int range;
    if (getenv("EXPLICIT")) {
        Result(int, ParseError) _range = parse_int(argv[1]);
        D(_range);
        if (! _range.is_ok) {
            DIE_("argument 1 %s", string_from_ParseError(_range.err));
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
}
