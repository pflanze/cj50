#include <cj50.h>

int main(int argc, char** argv) {
    Vec(CStr) vec = new_Vec_CStr();
    for (int i = 1; i < argc; i++) {
        push(&vec, CStr_from_cstr_unsafe(xstrdup(argv[i])));
    }
    Vec(CStr) vec2 = new_Vec_CStr();
    for (int i = argc; i > 0; i--) {
        push(&vec2, CStr_from_cstr_unsafe(xstrdup(argv[i - 1])));
    }
    append(&vec, &vec2);
    DBG(vec2);
    if (getenv("move")) {
        DBG(vec);
    } else {
        DBG(&vec);
        Vec(CStr) vec3 = new_Vec_CStr();
        while (true) {
            if_let_Some(AUTO v, pop(&vec)) {
                push(&vec3, v);
            } else_None {
                break;
            }
            DBG(&vec);
        }
        drop(vec);
        DBG(vec3);
    }
}
