#include <cj50.h>

bool render_squares(SDL_Renderer* renderer, void* context,
                    Vec2(int) window_dimensions) {
    float* t = context; // The time in seconds

    *t += 1./60.;
    float factor = sin(*t);

    set_draw_color(renderer, color(156, 0, 0));
    clear(renderer);
    set_draw_color(renderer, color(0, 156, 0));

    const int num_squares = 70;
    int dy_per_square = window_dimensions.y * 0.9 / (float)num_squares;
    int square_side_len = dy_per_square * 0.8;
    Vec2(float) diagonal = { square_side_len, square_side_len };
    for (int i = 0; i < num_squares; i++) {
        Vec2(float) topleft = {
            (window_dimensions.x / 2 +
             factor * window_dimensions.x * 0.4 * i / num_squares),
            i * dy_per_square
        };
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
