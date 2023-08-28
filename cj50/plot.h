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
} Pixels_float;

static
size_t _pixels_size_for_Pixels_float(Vec2(int) geometry, size_t siz) {
    return geometry.x * geometry.y * siz;
}

static
void clear_Pixels_float(Pixels_float *pixels) {
    // OK for float 0.0?
    memset(pixels->pixels,
           0,
           _pixels_size_for_Pixels_float(pixels->geometry, sizeof(Vec3(float))));
}

static
Pixels_float new_Pixels_float(Vec2(int) geometry) {
    AUTO pixels = (Pixels_float) {
        .geometry = geometry,
        .pixels = xmalloc(_pixels_size_for_Pixels_float(geometry, sizeof(Vec3(float)))),
    };
    clear_Pixels_float(&pixels);
    return pixels;
}

static
void drop_Pixels_float(Pixels_float self) {
    free(self.pixels);
}

static
void possibly_resize_Pixels_float(Pixels_float *self, Vec2(int) geometry) {
    if (equal_Vec2_int(&geometry, &self->geometry)) {
        // noop
    } else {
        // hmm, evil ownership management?
        drop_Pixels_float(*self);
        *self = new_Pixels_float(geometry);
    }
}


static
Vec3(float)* at_Pixels_float(Pixels_float * RESTRICT pixels, Vec2(int) point) {
    assert(point.x < pixels->geometry.x);
    assert(point.y < pixels->geometry.y);
    return &pixels->pixels[point.x + point.y * pixels->geometry.x];
}

static
void draw_point_Pixels_float(Pixels_float * RESTRICT pixels,
                             Vec2(float) point,
                             Vec3(float) color,
                             // The maximum brightness seen of any color:
                             float * RESTRICT max_color_lum) {
    int _x = point.x + 0.5f;
    int _y = point.y + 0.5f;
    // Add light to 9 positions at and around this point.
    for (int y = _y - 1; y <= _y + 1; y++) {
        if ((y >= 0) && (y < pixels->geometry.y)) {
            int xbase = _x - 1;
            if ((xbase >= 0) && ((xbase + 2) < pixels->geometry.x)) {
                for (int xadd = 0; xadd < 3; xadd++) {
                    int x = xbase + xadd;
                    float xf = x;
                    float yf = y;
                    float attenuation = 1.f / MAX(
                        0.32f, // smaller -> skinnier lines
                        square(xf - point.x) + square(yf - point.y));
                    Vec3(float) lumplus =
                        mul_Vec3_float_float(color, attenuation);
                    Vec3(float)* RESTRICT lumtot =
                        at_Pixels_float(pixels, vec2_int(x, y));
                    Vec3(float) lumtotnew = add_Vec3_float(*lumtot, lumplus);
                    *lumtot = lumtotnew;
#define UPDATE_MAX(field)                                           \
                    if (lumtotnew.field > *max_color_lum) {     \
                        *max_color_lum = lumtotnew.field;       \
                    }
                    UPDATE_MAX(x);
                    UPDATE_MAX(y);
                    UPDATE_MAX(z);
#undef UPDATE_MAX
                }
            } else {
                // slow fallback
                for (int x = _x - 1; x <= _x + 1; x++) {
                    if ((x >= 0) && (x < pixels->geometry.x)) {
                        float xf = x;
                        float yf = y;
                        float attenuation = 1.f / MAX(
                            0.32f, // smaller -> skinnier lines
                            square(xf - point.x) + square(yf - point.y));
                        Vec3(float) lumplus =
                            mul_Vec3_float_float(color, attenuation);
                        Vec3(float)* RESTRICT lumtot =
                            at_Pixels_float(pixels, vec2_int(x, y));
                        Vec3(float) lumtotnew = add_Vec3_float(*lumtot, lumplus);
                        *lumtot = lumtotnew;
#define UPDATE_MAX(field)                                           \
                        if (lumtotnew.field > *max_color_lum) {     \
                            *max_color_lum = lumtotnew.field;       \
                        }
                        UPDATE_MAX(x);
                        UPDATE_MAX(y);
                        UPDATE_MAX(z);
#undef UPDATE_MAX
                    }
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
    VertexRenderer rdr;
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
        new_VertexRenderer()
    };
}

static UNUSED
void drop_PlotrenderCtx(PlotrenderCtx self) {
    drop_VertexRenderer(self.rdr);
    drop_Pixels_float(self.pixels);
}


static
u8 u8_from_float(float x, float max) {
    if (x <= 0.f) {
        return 0;
    } else if (x >= max) {
        return 255;
    } else {
        // 'linear': return x * 255.f / max;
        // better show darks:
        return 255 * sqrt(x) / sqrt(max);
    }
}

static
float squared_color_float_from_u8(u8 col) {
    return square(col);
}

const bool showdebug = false;

static UNUSED
bool plot_render(SDL_Renderer* renderer, void* RESTRICT _ctx,
                 Vec2(int) window_dimensions) {
    PlotrenderCtx* RESTRICT ctx = _ctx;
    Rect2(float)* RESTRICT viewport = &ctx->viewport;
    Vec2(float) start = viewport->start;
    Vec2(float) extent = viewport->extent;
    possibly_resize_Pixels_float(&ctx->pixels, window_dimensions);

    asserting_sdl(SDL_SetRenderDrawColor(renderer, 0,0,0, 128));
    asserting_sdl(SDL_RenderClear(renderer));

    float width = window_dimensions.x;
    float height = window_dimensions.y;

    // clear_Pixels_float(&ctx->pixels); -- now it's cleared below from previous frame already
    float max_color_lum = 0;

    for (size_t j = 0; j < ctx->functions.len; j++) {
        Color color = ctx->functions.ptr[j].color;
        Vec3(float) colorf = vec3_float(squared_color_float_from_u8(color.r),
                                        squared_color_float_from_u8(color.g),
                                        squared_color_float_from_u8(color.b));
        Option(float)(*f)(float) = ctx->functions.ptr[j].f;

        const int oversampling = 16;
        // Is dx approach still OK with float and oversampling?
        const float dx = extent.x / (width * oversampling);
        float x = start.x;
        for (int i = 0; i < window_dimensions.x * oversampling; i++) {
            if_let_Some(y, f(x)) {
                float xscreen = i / oversampling;
                float yscreen = (y - start.y) / extent.y * height;
                draw_point_Pixels_float(&ctx->pixels,
                                        vec2_float(xscreen, height - yscreen),
                                        colorf,
                                        &max_color_lum);
            } else_None;
            x += dx;
        }
    }

    VertexRenderer *rdr = &ctx->rdr;
    clear_VertexRenderer(rdr);

    Vec3(float) * RESTRICT pixels = ctx->pixels.pixels; // vec3(R, G, B) [x + y*width]
    for (int y = 0; y < window_dimensions.y; y++) {
        for (int x = 0; x < window_dimensions.x; x++) {
            Vec3(float) p = pixels[x + y * window_dimensions.x]; // XX vs at_Pixels_float ?
            if ((p.x == 0.f) && (p.y == 0.f) && (p.z == 0.f)) {
                // noop (most of the pixels)
            } else {
                AUTO col = ColorA(u8_from_float(p.x, max_color_lum),
                                  u8_from_float(p.y, max_color_lum),
                                  u8_from_float(p.z, max_color_lum),
                                  255);
                int p1 = push_vertex(rdr, vertex_2(vec2_float(x, y), col));
                int p2 = push_vertex(rdr, vertex_2(vec2_float(x+1, y), col));
                int p3 = push_vertex(rdr, vertex_2(vec2_float(x, y+1), col));
                int p4 = push_vertex(rdr, vertex_2(vec2_float(x+1, y+1), col));
                push_triangle(rdr, vec3_int(p1, p2, p3));
                push_triangle(rdr, vec3_int(p2, p3, p4));
                // Clear pixel for next frame, already, cheaper than memset ?:
                pixels[x + y * window_dimensions.x] = vec3_float(0.f, 0.f, 0.f);
            }
        }
    }

    render_VertexRenderer(renderer, rdr);

    return true;
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

