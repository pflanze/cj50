#include <cj50.h>

int main(int argc, char** argv) {
    assert(argc == 2);
    cstr path = argv[1];
    if_let_Ok(cnt, filecontents_String(path, 10000)) {
        print(&cnt);
        drop(cnt);
    } else_Err(e) {
        fprintln(stderr, &e);
        drop(e);
        exit(1);
    } end_let_Ok;
}
