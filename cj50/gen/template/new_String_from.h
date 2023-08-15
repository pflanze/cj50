// parameters: T, FORMATSTRING

/// Create a String from a number.

static UNUSED
String XCAT(new_String_from_move_, T)(T v) {
#define BUFSIZE 50
    char buf[BUFSIZE];
    size_t did = snprintf(buf, BUFSIZE, FORMATSTRING, v);
    assert(did < BUFSIZE);
    return (String) {
        .vec = (Vec(char)) {
            .ptr = xmemcpy(buf, did + 1),
            .cap = did + 1,
            .len = did
        }
    };
#undef BUFSIZE
}
