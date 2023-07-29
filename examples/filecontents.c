#include <cj50.h>

int main(int argc, char** argv) {
    assert(argc == 2);
    string path = argv[1];
    Result(string, SystemError) rcnt = filecontents_string(path);
    // DBG(rcnt);
    string cnt = unwrap(rcnt);
    print(cnt);
    drop(cnt);
}
