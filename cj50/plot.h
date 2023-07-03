#pragma once

#include <SDL2/SDL.h>
#include <cj50/sdlutil.h>
#include <cj50/float.h>

struct PlotrenderCtx {
    Option(float)(*f)(float);
    int width;
    int height;
    Rect2 viewport;
};

bool plot_render(SDL_Renderer* renderer, void* _ctx) {
    struct PlotrenderCtx* ctx = _ctx;
    Rect2* viewport = &ctx->viewport;
    Vec2 start = viewport->start;
    Vec2 extent = viewport->extent;
    Option(float)(*f)(float) = ctx->f;

    assert_sdl(SDL_SetRenderDrawColor(renderer, 0,0,0, 128));
    assert_sdl(SDL_RenderClear(renderer));
    assert_sdl(SDL_SetRenderDrawColor(renderer, 240,220,0, 128));

    Vec2 diagonal = { 2, 2 };
    float w = ctx->width;
    float h = ctx->height;
    for (int i = 0; i < ctx->width; i++) {
        float x = start.x + extent.x / w * i;
        Option(float) y = f(x);
        if (y.is_some) {
            /* D(vec2(x, y.value)); */
            int yint = (y.value - start.y) / extent.y * h;
            Vec2 pos = { i, ctx->height - yint };
            /* D(pos); */
            Rect2 r = { pos, diagonal };
            SDL_Rect sr = to_sdl(r);
            assert_sdl(SDL_RenderDrawRect(renderer, &sr));
        }
    }

    return true;
}


/// Show a window with a 2-dimensional plot of the function
/// `f`. `viewport` is the (initial) range of coordinates that is
/// shown on the screen.
int plot_float(Option(float)(*f)(float), Rect2 viewport) {
    const int width = 800;
    const int height = 600;

    struct PlotrenderCtx ctx = {
        f, width, height, viewport
    };

    graphics_render(width, height, plot_render, &ctx);

    return 0;
}

