#include <cj50.h>

int main(int argc, string* argv) {
    if (argc != 2) {
        DIE("usage: random range");
    }
    ParseResult(int) range = parse_int(argv[1]);
    if (! range.is_ok) {
        DIE_("argument 1 %s", string_from_ParseError(range.err));
    }
    for (int i = 0; i < 10; i++) {
        int r = get_random_int(range.ok);
        print(r);
        print("\n");
        assert(r >= 0);
        assert(r < range.ok);
    }
}
