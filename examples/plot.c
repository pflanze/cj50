#include <cj50.h>

Option(float) inverse(float x) {
    return (x == 0) ? none(typeof(x)) : some(.01f / x);
}

Option(float) sine(float x) {
    return some(sinf(x));
}

int main() {
    if (getenv("SINGLEFUNCTION")) {
        plot_function_float(sine, rect2(vec2(-2, -2), vec2(4, 4)));
    } else {

#define NUMFS 2
        ColorFunction_float fs[NUMFS] = {
            { color(250, 80, 0), inverse },
            { color(0, 120, 240), sine },
        };

        plot_functions_float(NUMFS, fs, rect2(vec2(-2, -2), vec2(4, 4)));
#undef NUMFS
    }
}
