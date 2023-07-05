#include <cj50.h>

const int screen_width = 640;
const int screen_height = 480;

bool render_squares(SDL_Renderer* renderer, void* context) {
    float* t = context; // The time in seconds

    *t += 1./60.;
    float factor = sin(*t);

    assert_sdl(SDL_SetRenderDrawColor(renderer, 156,0,0, 128));
    assert_sdl(SDL_RenderClear(renderer));
    assert_sdl(SDL_SetRenderDrawColor(renderer, 0,156,0, 128));

    Vec2 diagonal = { 5, 5 };
    for (int i = 0; i < 70; i++) {
        Vec2 topleft = { 300 + i * factor * 4, i * 6 };
        Rect2 r = { topleft, diagonal };
        SDL_Rect sr = to_sdl(r);
        assert_sdl(SDL_RenderDrawRect(renderer, &sr));
    }

    return true;
}

int main() {
    float t = 0;
    graphics_render(screen_width, screen_height, render_squares, &t);
}
