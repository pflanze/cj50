#include <cj50.h>

const int window_width = 640;
const int window_height = 480;

bool render_squares(SDL_Renderer* renderer, void* context) {
    float* t = context; // The time in seconds

    *t += 1./60.;
    float factor = sin(*t);

    set_draw_color(renderer, color(156, 0, 0));
    clear(renderer);
    set_draw_color(renderer, color(0, 156, 0));

    Vec2 diagonal = { 5, 5 };
    for (int i = 0; i < 70; i++) {
        Vec2 topleft = { 300 + i * factor * 4, i * 6 };
        Rect2 r = { topleft, diagonal };
        draw_rect(renderer, r);
    }

    return true;
}

int main() {
    float t = 0;
    graphics_render(window_width, window_height, render_squares, &t);
}
