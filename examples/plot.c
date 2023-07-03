#include <cj50.h>

Option(float) inverse(float x) {
    return (x == 0) ? none_float() : some(.01f / x);
}

int main() {
    plot_float(inverse, rect2(vec2(-2, -2), vec2(4, 4)));
}
