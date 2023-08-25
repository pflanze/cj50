#include <cj50.h>

bool render_squares(SDL_Renderer* renderer, void* context,
                    Vec2(int) window_dimensions) {
    float* t = context; // The time in seconds

    *t += 1./60.;
    float factor = sin(*t);

    set_draw_color(renderer, color(156, 0, 0));
    clear(renderer);
    set_draw_color(renderer, color(0, 156, 0));

    Vec2(float) diagonal = { 5, 5 };
    for (int i = 0; i < 70; i++) {
        Vec2(float) topleft = { 300 + i * factor * 4, i * 6 };
        Rect2(float) r = { topleft, diagonal };
        draw_rect(renderer, r);
    }

    return true;
}

int main() {
    if (getenv("NOGRAPHICS")) {
        return 0;
    }

    float t = 0;
    graphics_render("Graphics",
                    vec2_int(640, 480), render_squares, &t);
}
