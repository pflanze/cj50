#include <cj50.h>

int main(int argc, char** argv) {
    assert(argc == 2);
    string path = argv[1];
    Result(String, SystemError) rcnt = filecontents_String(path);
    // DBG(rcnt);
    String cnt = unwrap(rcnt);
    print(cnt);
    drop(cnt);
}
