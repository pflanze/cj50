#include <cj50.h>

int main(int argc, char** argv) {
    assert(argc == 2);
    cstr path = argv[1];
    if_let_Ok(String cnt, filecontents_String(path)) {
        print(cnt);
        drop(cnt);
    } else_Err(SystemError e) {
        fprintln_SystemError(stderr, e);
        exit(1);
    } end_let_Ok;
}
