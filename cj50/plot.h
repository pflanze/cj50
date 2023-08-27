#pragma once

#include <SDL2/SDL.h>
#include <cj50/sdlutil.h>
#include <cj50/float.h>
#include <cj50/gen/Vec.h>
#include <cj50/instantiations/Vec2_u32.h>
#include <sys/param.h> /* MAX */
// for debugging only:
#include <cj50/String.h>


/// Combination of color and a function, for drawing multiple
/// functions in the same plot via `plot_functions_float`.
typedef struct ColorFunction_float {
    Color color;
    Option(float)(*f)(float);
} ColorFunction_float;

static UNUSED
void drop_ColorFunction_float(UNUSED ColorFunction_float v) {}

static UNUSED
bool equal_ColorFunction_float(const ColorFunction_float *a,
                               const ColorFunction_float *b) {
    return (equal_Color(&a->color, &b->color) &&
            a->f == b->f);
}

static UNUSED
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

// ------------------------------------------------------------------
// Pixel drawing

typedef struct ARGB8888 {
    u8 v[4];
} ARGB8888;

static UNUSED
ARGB8888 new_ARGB8888(u8 a, u8 r, u8 g, u8 b) {
    return (ARGB8888) { b, g, r, a };
}

static UNUSED
int print_debug_move_ARGB8888(ARGB8888 self) {
    // XX dbg only:
    //return print_debug_move_strslice(new_strslice((const char *)&self.v, 4));
    return printf("ARGB8888(%i, %i, %i, %i)", self.v[3], self.v[2], self.v[1], self.v[0]);
}


typedef struct Pixels_float {
    Vec2(int) geometry;
    Vec3(float) *pixels; // vec3(R, G, B) [x + y*width]
    ARGB8888 *pixels2;
} Pixels_float;

static
size_t _pixels_size_for_Pixels_float(Vec2(int) geometry, size_t siz) {
    return geometry.x * geometry.y * siz;
}

static
Pixels_float new_Pixels_float(Vec2(int) geometry) {
    return (Pixels_float) {
        .geometry = geometry,
        .pixels = xmalloc(_pixels_size_for_Pixels_float(geometry, sizeof(Vec3(float)))),
        .pixels2 = xmalloc(_pixels_size_for_Pixels_float(geometry, sizeof(ARGB8888))),
    };
}

static
void drop_Pixels_float(Pixels_float self) {
    free(self.pixels2);
    free(self.pixels);
}

static
void clear_Pixels_float(Pixels_float *pixels) {
    // OK for float 0.0?
    memset(pixels->pixels,
           0,
           _pixels_size_for_Pixels_float(pixels->geometry, sizeof(Vec3(float))));
}

static
Vec3(float)* at_Pixels_float(Pixels_float *pixels, Vec2(int) point) {
    assert(point.x < pixels->geometry.x);
    assert(point.y < pixels->geometry.y);
    return &pixels->pixels[point.x + point.y * pixels->geometry.x];
}

static
void draw_point_Pixels_float(Pixels_float *pixels,
                             Vec2(float) point,
                             Vec3(float) color,
                             // The maximum brightness seen of any color:
                             float *max_color_lum) {
    int _x = point.x + 0.5f;
    int _y = point.y + 0.5f;
    // Add light to 9 positions at and around this point.
    for (int y = _y - 1; y <= _y + 1; y++) {
        if ((y >= 0) && (y < pixels->geometry.y)) {
            for (int x = _x - 1; x <= _x + 1; x++) {
                if ((x >= 0) && (x < pixels->geometry.x)) {
                    float xf = x;
                    float yf = y;
                    float attenuation = 1.f / MAX(
                        0.32f, // smaller -> skinnier lines
                        square(xf - point.x) + square(yf - point.y));
                    Vec3(float) lumplus =
                        mul_Vec3_float_float(color, attenuation);
                    Vec3(float)* lumtot =
                        at_Pixels_float(pixels, vec2_int(x, y));
                    Vec3(float) lumtotnew = add_Vec3_float(*lumtot, lumplus);
#define UPDATE_MAX(field)                               \
                    if (lumtotnew.field > *max_color_lum) {     \
                        *max_color_lum = lumtotnew.field;       \
                    }
                    UPDATE_MAX(x);
                    UPDATE_MAX(y);
                    UPDATE_MAX(z);
#undef UPDATE_MAX
                    *lumtot = lumtotnew;
                }
            }
        }
    }
}

// ------------------------------------------------------------------

typedef struct PlotrenderCtx {
    slice(ColorFunction_float) functions;
    Rect2(float) viewport;
    Pixels_float pixels;
    Option(Texture) texture; // initialized from the render function,
                             // ugly.
} PlotrenderCtx;

static UNUSED
PlotrenderCtx new_PlotrenderCtx(
    slice(ColorFunction_float) functions,
    Rect2(float) viewport,
    Vec2(int) geometry
    )
{
    return (PlotrenderCtx) {
        functions,
        viewport,
        new_Pixels_float(geometry),
        NONE
    };
}

static UNUSED
void drop_PlotrenderCtx(PlotrenderCtx self) {
    drop_Pixels_float(self.pixels);
    drop_Option_Texture(self.texture);
}


static UNUSED
u8 u8_from_float(float x, float max) {
    if (x <= 0.f) {
        return 0;
    } else if (x >= max) {
        return 255;
    } else {
        return x *  255.f / max; //X XX
    }
}

const bool showdebug = false;

static UNUSED
bool plot_render(SDL_Renderer* renderer, void* _ctx, Vec2(int) window_dimensions) {
    PlotrenderCtx* ctx = _ctx;
    Texture texture;
    if_let_Some(t, ctx->texture) {
        texture = t; // Copy
    } else_None {
        // Create the Texture now that we have access to the renderer;
        // ugly.
        /* WARN("init Option(Texture)"); */
        texture = create_Texture(renderer,
                                 SDL_PIXELFORMAT_BGRA32, // SDL_PIXELFORMAT_ARGB32, // SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 ctx->pixels.geometry);
        ctx->texture = Some(Texture)(texture);
    }
    /* print_debug_Texture(&texture); */
    Rect2(float)* viewport = &ctx->viewport;
    Vec2(float) start = viewport->start;
    Vec2(float) extent = viewport->extent;

    asserting_sdl(SDL_SetRenderDrawColor(renderer, 0,0,0, 128));
    asserting_sdl(SDL_RenderClear(renderer));

    float width = window_dimensions.x;
    float height = window_dimensions.y;

    clear_Pixels_float(&ctx->pixels);
    float max_color_lum = 0;

    for (size_t j = 0; j < ctx->functions.len; j++) {
        Color color = ctx->functions.ptr[j].color;
        Vec3(float) colorf = vec3_float(color.r,
                                        color.g,
                                        color.b);
        Option(float)(*f)(float) = ctx->functions.ptr[j].f;

        const int oversampling = 16;
        // Is dx approach still OK with float and oversampling?
        const float dx = extent.x / (width * oversampling);
        float x = start.x;
        for (int i = 0; i < window_dimensions.x * oversampling; i++) {
            if_let_Some(y, f(x)) {
                float xscreen = i / oversampling;
                float yscreen = (y - start.y) / extent.y * height;
                Vec2(float) point = { xscreen, height - yscreen };
                draw_point_Pixels_float(&ctx->pixels,
                                        point,
                                        colorf,
                                        &max_color_lum);
            } else_None;
            x += dx;
        }
    }

    Vec3(float) *pixels = ctx->pixels.pixels; // vec3(R, G, B) [x + y*width]
    ARGB8888 *pixels2 = ctx->pixels.pixels2;
    size_t numpixels = window_dimensions.x * window_dimensions.y;
    for (size_t i = 0; i < numpixels; i++) {
        Vec3(float) p = pixels[i];
        ARGB8888 p2 = new_ARGB8888(
            255,
            /* i % ctx->pixels.geometry.x, */
            /* i % ctx->pixels.geometry.x, // width  = float, */
            /* 120 */
            u8_from_float(p.x, max_color_lum),
            u8_from_float(p.y, max_color_lum),
            u8_from_float(p.z, max_color_lum)
            /* 0., */
            /* 200., */
            /* 255. */
            );
        pixels2[i] = p2;
    }
    if (showdebug) {
        for (size_t i = 0; i < numpixels; i++) {
            print_debug_Vec3_float(&pixels[i]);
            print_move_cstr(" = ");
            print_debug_move_ARGB8888(pixels2[i]);
            print_move_cstr("\n");
        }
    }

    update_Texture(&texture,
                   None(Rect2(int)),
                   pixels2,
                   window_dimensions.x * sizeof(ARGB8888));

    render_Texture(renderer,
                   &texture,
                   None(Rect2(int)),
                   None(Rect2(int)));
    return !showdebug;
}



/// Show a window with a 2-dimensional plot of the `num_fs` given
/// functions, which expect a float and return an optional float
/// when possible. `viewport` is the (initial) range of coordinates
/// that is shown on the screen.
static UNUSED
int plot_functions_float(slice(ColorFunction_float) fs,
                         Rect2(float) viewport) {
    AUTO geometry = vec2_int(800, 600);

    PlotrenderCtx ctx = new_PlotrenderCtx(fs, viewport, geometry);
    graphics_render("Plot functions",
                    geometry, plot_render, &ctx);
    drop_PlotrenderCtx(ctx);
    return 0;
}


/// Show a window with a 2-dimensional plot of the function `f`, which
/// expects a float and returns an optional float when
/// possible. `viewport` is the (initial) range of coordinates that is
/// shown on the screen.
static UNUSED
int plot_function_float(Option(float)(*f)(float), Rect2(float) viewport) {
    AUTO geometry = vec2_int(800, 600);

    DEF_SLICE(ColorFunction_float, fs,
              {
                  { color(240, 220, 0), f }
              });

    PlotrenderCtx ctx = new_PlotrenderCtx(fs, viewport, geometry);
    graphics_render("Plot function",
                    geometry, plot_render, &ctx);
    drop_PlotrenderCtx(ctx);
    return 0;
}

