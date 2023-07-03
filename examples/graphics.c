#include <cj50.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool render_squares(SDL_Renderer* renderer, void* context) {
    float* t = context;

    *t += 0.01;
    float factor = sin(*t);

    assert_sdl(SDL_SetRenderDrawColor(renderer, 156,0,0, 128));
    assert_sdl(SDL_RenderClear(renderer));
    assert_sdl(SDL_SetRenderDrawColor(renderer, 0,156,0, 128));

    Vec2 diagonal = { 5, 5 };
    for (int i = 0; i < 70; i++) {
        Vec2 topleft = { 300 + i * factor * 4, i * 6 };
        Rect2 r = { topleft, diagonal };
        /* D(r); */
        SDL_Rect sr = to_sdl(r);
        assert_sdl(SDL_RenderDrawRect(renderer, &sr) < 0);
    }

    return true;
}


int main(/* int argc, char* args[] */) {
    // XX vec2(SCREEN_WIDTH, SCREEN_HEIGHT)
    float t = 0;
    graphics_render(SCREEN_WIDTH, SCREEN_HEIGHT, render_squares, &t);
}
