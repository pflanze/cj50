#pragma once

#include <SDL2/SDL.h>
#include <cj50/sdlutil.h>
#include <cj50/float.h>
#include <cj50/gen/Vec.h>


/// Combination of color and a function, for drawing multiple
/// functions in the same plot via `plot_functions_float`.
typedef struct ColorFunction_float {
    Color color;
    Option(float)(*f)(float);
} ColorFunction_float;

static
void drop_ColorFunction_float(UNUSED ColorFunction_float v) {}

static
bool equal_ColorFunction_float(const ColorFunction_float *a,
                               const ColorFunction_float *b) {
    return (equal_Color(&a->color, &b->color) &&
            a->f == b->f);
}

static
int print_debug_ColorFunction_float(const ColorFunction_float *v) {
    INIT_RESRET;
    RESRET(printf("ColorFunction_float("));
    RESRET(print_debug_Color(&v->color));
    RESRET(printf(", %p)", v->f));
cleanup:
    return ret;
}

GENERATE_Option(ColorFunction_float);

#define T ColorFunction_float
#include <cj50/gen/template/Vec.h>
#undef T


struct PlotrenderCtx {
    slice(ColorFunction_float) functions;
    int width;
    int height;
    Rect2 viewport;
};

bool plot_render(SDL_Renderer* renderer, void* _ctx) {
    struct PlotrenderCtx* ctx = _ctx;
    Rect2* viewport = &ctx->viewport;
    Vec2 start = viewport->start;
    Vec2 extent = viewport->extent;

    assert_sdl(SDL_SetRenderDrawColor(renderer, 0,0,0, 128));
    assert_sdl(SDL_RenderClear(renderer));

    Vec2 diagonal = { 2, 2 };
    float w = ctx->width;
    float h = ctx->height;

    for (size_t j = 0; j < ctx->functions.len; j++) {
        Color color = ctx->functions.ptr[j].color;
        Option(float)(*f)(float) = ctx->functions.ptr[j].f;
        
        assert_sdl(SDL_SetRenderDrawColor(renderer,
                                          color.r,
                                          color.g,
                                          color.b,
                                          128));

        for (int i = 0; i < ctx->width; i++) {
            float x = start.x + extent.x / w * i;
            Option(float) y = f(x);
            if (y.is_some) {
                /* DBG(vec2(x, y.value)); */
                int yint = (y.value - start.y) / extent.y * h;
                Vec2 pos = { i, ctx->height - yint };
                /* DBG(pos); */
                Rect2 r = { pos, diagonal };
                SDL_Rect sr = to_sdl(r);
                assert_sdl(SDL_RenderDrawRect(renderer, &sr));
            }
        }
    }
    
    return true;
}



/// Show a window with a 2-dimensional plot of the `num_fs` given
/// functions, which expect a float and return an optional float
/// when possible. `viewport` is the (initial) range of coordinates
/// that is shown on the screen.
int plot_functions_float(slice(ColorFunction_float) fs,
                         Rect2 viewport) {
    const int width = 800;
    const int height = 600;

    struct PlotrenderCtx ctx = {
        fs, width, height, viewport
    };

    graphics_render("Plot multiple functions",
                    width, height, plot_render, &ctx);

    return 0;
}


/// Show a window with a 2-dimensional plot of the function `f`, which
/// expects a float and returns an optional float when
/// possible. `viewport` is the (initial) range of coordinates that is
/// shown on the screen.
int plot_function_float(Option(float)(*f)(float), Rect2 viewport) {
    const int width = 800;
    const int height = 600;

    DEF_SLICE(ColorFunction_float, fs,
              {
                  { color(240, 220, 0), f }
              });

    struct PlotrenderCtx ctx = {
        fs,
        width,
        height,
        viewport
    };

    graphics_render("Plot single function", width, height, plot_render, &ctx);

    return 0;
}

