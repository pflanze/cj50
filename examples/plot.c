#include <cj50.h>

Option(float) inverse(float x) {
    return (x == 0) ? none(typeof(x)) : some(.01f / x);
}

Option(float) sine(float x) {
    return some(sinf(x));
}

int main() {
    if (getenv("NOGRAPHICS")) {
        return 0;
    }

    if (getenv("SINGLEFUNCTION")) {
        plot_function_float(sine, rect2(vec2_float(-2, -2),
                                        vec2_float(4, 4)));
    } else {

        DEF_SLICE(ColorFunction_float, fs,
                  {
                      { color(250, 80, 0), inverse },
                      { color(0, 120, 240), sine },
                  });

        plot_functions_float(fs, rect2(vec2_float(-2, -2),
                                       vec2_float(4, 4)));
    }
}
