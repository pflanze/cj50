#pragma once

//! Utilities for working with the SDL2 library.

//! This directly uses (only) some of the types that the SDL2 library defines;
//! and it offers functions that follow the style and use the types from the
//! cj50 library, to make programming graphics nicer. It might not have some
//! functionality that you want, though, in which case it might be useful to
//! check the [SDL2 documentation](https://wiki.libsdl.org/SDL2/FrontPage);
//! functions on `SDL_Renderer` for example can easily be called from a
//! `renderframe` function (as passed to `graphics_render`). Mostly that
//! shouldn't be necessary, though; if missing, it should be added to cj50.

//! But note that SDL2 doesn't offer many shapes, either; if you need
//! complicated shapes, the way to do that with SDL2 (as with realtime graphics
//! programming in general) is to compose the drawing from triangles--this
//! offers complete freedom about the shape and its coloring and is still
//! efficient. cj50 offers an abstraction, `VertexRenderer` (which is built on
//! top of SDL2's `SDL_RenderGeometry`), to do this. The way how this works is
//! to first call `new_VertexRenderer`, then add vertices (points with colors)
//! to it and then add triangles build from those vertices; then when your shape
//! is complete, you call `render_VertexRenderer` to send it to the screen. See
//! [examples/vertexgraphics.c](../examples/vertexgraphics.c) for an example.

//! (TODO) The functions around Texture aren't worked out well yet.

#include <SDL2/SDL.h>
#include <cj50/basic-util.h>
#include <cj50/CStr.h>
#include <cj50/math.h>
#include <cj50/instantiations/Vec_Vec2_int.h>
#include <cj50/instantiations/Vec_Vec2_float.h>
#include <cj50/instantiations/Vec_Rect2_float.h>
#include <cj50/instantiations/Vec_Vec3_int.h>
#include <cj50/instantiations/Vec2_u32.h>
#include <cj50/gen/Vec.h>
#include <cj50/gen/ref.h>
#include <cj50/resret.h>
#include <time.h>


bool sdlutil_debug = false;

// ------------------------------------------------------------------
// had this in scientific repo already  todo check

#define GETTIME(ref) \
    assert(clock_gettime(CLOCK_MONOTONIC, ref) == 0)

#define DBG_TIMESPEC(var)                                       \
    printf(#var " = %li.%09li\n", var.tv_sec, var.tv_nsec);
        
#define TIMESPEC_60th()                                           \
    ((struct timespec) { .tv_sec = 0, .tv_nsec = 16666666 })

#define NULL_TIMESPEC()                           \
    ((struct timespec) {                        \
        .tv_sec = 0,                            \
        .tv_nsec = 0                            \
    })

static UNUSED
void timespec_assert_sane(struct timespec *self) {
    assert(self->tv_nsec >= 0);
    assert(self->tv_nsec < 1000000000);
}

static UNUSED
struct timespec timespec_add(struct timespec a, struct timespec b) {
    timespec_assert_sane(&a);
    timespec_assert_sane(&b);
    uint64_t ansec = a.tv_nsec;
    uint64_t bnsec = b.tv_nsec;
    uint64_t nsec = ansec + bnsec;
    time_t sec = a.tv_sec + b.tv_sec;
    // XX what when b is negative?
    if (nsec > 1000000000) {
        sec++;
        nsec -= 1000000000;
    }
    return (struct timespec) {
        .tv_sec = sec,
        .tv_nsec = nsec
    };
}

// If `a >= b` it returns a - b, otherwise returns the time
// 0. Which is what we want for sleep.
static UNUSED
struct timespec timespec_sub(struct timespec a, struct timespec b) {
    timespec_assert_sane(&a);
    timespec_assert_sane(&b);
    if (a.tv_sec < b.tv_sec) {
        return NULL_TIMESPEC();
    } else if (a.tv_sec == b.tv_sec) {
        if (a.tv_nsec <= b.tv_nsec) {
            return NULL_TIMESPEC();
        }
    }
    time_t sec = a.tv_sec - b.tv_sec;
    int64_t ansec = a.tv_nsec;
    int64_t bnsec = b.tv_nsec;
    int64_t nsec = ansec - bnsec;
    if (nsec < 0) {
        sec--;
        nsec += 1000000000;
    }
    return (struct timespec) {
        .tv_sec = sec,
        .tv_nsec = nsec
    };
}

static
bool timespec_is_zero(struct timespec *self) {
    return (self->tv_sec == 0)
        && (self->tv_nsec == 0);
}

// ------------------------------------------------------------------

static UNUSED
SDL_Point to_sdl_Vec2_int(Vec2(int) self) {
    return (SDL_Point) { self.x, self.y };
}

static UNUSED
SDL_FPoint to_sdl_Vec2_float(Vec2(float) self) {
    return (SDL_FPoint) { self.x, self.y };
}

static UNUSED
SDL_Rect to_sdl_Rect2_int(Rect2(int) r) {
    return (SDL_Rect) { r.start.x, r.start.y, r.extent.x, r.extent.y };
}

static UNUSED
SDL_FRect to_sdl_Rect2_float(Rect2(float) r) {
    return (SDL_FRect) { r.start.x, r.start.y, r.extent.x, r.extent.y };
}


/// Convert a value of a type from cjmath.h into a type from SDL.h
#define to_sdl(v)                                  \
    _Generic((v)                                   \
             , Vec2(int): to_sdl_Vec2_int          \
             , Vec2(float): to_sdl_Vec2_float      \
             , Rect2(int): to_sdl_Rect2_int        \
             , Rect2(float): to_sdl_Rect2_float    \
        )(v)





static UNUSED
int asserting_sdl_int(int code) {
    if (code < 0) {
        DIE_("SDL error: %s", SDL_GetError());
    } else {
        return code;
    }
}

static UNUSED
void _assert_sdl_pointer(void *p) {
    if (!p) {
        DIE_("SDL error: %s", SDL_GetError());
    }
}

#define DEF_ASSERTING_SDL_POINTER(T)                            \
    static UNUSED                                               \
    T* XCAT(asserting_sdl_pointer_, T)(T* p) {                  \
        _assert_sdl_pointer(p);                                 \
        return p;                                               \
    }

DEF_ASSERTING_SDL_POINTER(SDL_Surface);
DEF_ASSERTING_SDL_POINTER(SDL_Texture);

/// Assert that the return value of a function from the SDL2 libray
/// does not represent an error. If it is an error, print the SDL
/// error message and abort. Otherwise, return the value.

#define asserting_sdl(v)                                        \
    _Generic((v)                                                \
             , int: asserting_sdl_int                           \
             , SDL_Surface *: asserting_sdl_pointer_SDL_Surface \
             , SDL_Texture *: asserting_sdl_pointer_SDL_Texture \
        )(v)


/// Sleep the given duration in seconds. Note: this will block the
/// current thread for that duration, and if that thread is updating
/// the display or reacting to keyboard input, it will not do so for
/// the given duration.
static UNUSED
void sleep_float(float duration_seconds) {
    if (duration_seconds <= 0) {
        return;
    }
    float wholesec;
    float partialsec = modff(duration_seconds, &wholesec);
    struct timespec duration = {
        wholesec,
        partialsec * 1000000000.
    };
    struct timespec t;
    GETTIME(&t);
    struct timespec end_time = timespec_add(t, duration);
    while (true) {
        GETTIME(&t);
        struct timespec wait_time = timespec_sub(end_time, t);
        if (timespec_is_zero(&wait_time)) {
            break;
        }
        nanosleep(&wait_time, NULL); // XX error check?
    }
}


float rendersleep_seconds = 0.01;

/// Sleep for the number of seconds (as a float) stored in the global
/// `rendersleep_seconds` variable (0.01 seconds by default), but first flush
/// the past drawing commands to the display (via `SDL_RenderPresent`). This can
/// be useful for debugging purposes to track how a program is drawing.

/// Note: this (obviously?) won't show any `VertexRenderer` that the program
/// might have created and been filling; the program is responsible to call
/// `render_VertexRenderer` on those before calling `rendersleep`, if you want
/// to see their contents.

static UNUSED
void rendersleep(SDL_Renderer *rdr) {
    SDL_RenderPresent(rdr);
    sleep_float(rendersleep_seconds);
}
   


/// Open a window with the given window size, and call `renderframe`
/// about 60 times per second to draw a new image each
/// time.

/// `renderframe` is called with a `SDL_Renderer*` that needs to
/// be passed to the drawing functions like `set_draw_color`, and the
/// `context` value that was given to `graphics_render`; if you need to store
/// variables between subsequent frames, this is where you can put
/// them. The third argument to `renderframe` are the current window dimensions.

/// If `renderframe` returns `true`, the image is drawn and
/// `renderframe` is called again 1/60 seconds later. If it returns
/// `false`, the drawing stops, the window is closed, and
/// `graphics_render` returns.

static UNUSED
void graphics_render(cstr title,
                     Vec2(int) window_dimensions,
                     bool (*renderframe)(SDL_Renderer*, void*, Vec2(int)),
                     void* context)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        DIE_("SDL could not initialize! SDL_Error: %s",
             SDL_GetError());
    }

    asserting_sdl(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1));
    asserting_sdl(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8));

    Vec2(int) current_window_dimensions = window_dimensions;
    SDL_Window* window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        window_dimensions.x,
        window_dimensions.y,
        SDL_WINDOW_SHOWN);
    if (!window) {
        DIE_("Window could not be created! SDL_Error: %s",
             SDL_GetError());
    }
    SDL_SetWindowResizable(window, SDL_TRUE);

    /*
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

    //Fill the surface white
    SDL_FillRect(screenSurface,
                 NULL,
                 SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
    
    //Update the surface
    SDL_UpdateWindowSurface(window);
    */
    bool need_sleep; // accelerated renderer doesn't, software does
    SDL_Renderer * renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer) {
        need_sleep = false;
    } else {
        char* err1 = xstrdup(SDL_GetError());
        renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_SOFTWARE);
        if (!renderer) {
            DIE_("Software renderer could not be created (SDL_Error): %s\n"
                 "  after an accelerated renderer could not be created: %s\n",
                 SDL_GetError(),
                 err1);
        }
        free(err1);
        need_sleep = true;
    }
    if (need_sleep && ! getenv("SILENT")) {
        print_move_cstr("Note: using slower fallback software renderer.\n");
    }

    struct timespec last_t;
    GETTIME(&last_t);
    /* DBG_TIMESPEC(last_t); */
    
    SDL_Event e;
    bool quit = false;
    bool is_fullscreen = 0;
    while (!quit) {
        // Copy current window size on every frame, because the window
        // can be resized by the window manager (Alt-F11 to force it
        // out of full-screen mode).
        SDL_GetWindowSize(window,
                          &current_window_dimensions.x,
                          &current_window_dimensions.y);

        if (renderframe(renderer, context, current_window_dimensions)) {
            SDL_RenderPresent(renderer);
        } else {
            quit = true;
        }

        while (SDL_PollEvent(&e)) {
            /* DBG(e.type); */
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_TEXTINPUT) {
                /* DBG(e.text.text); */
                // ah, string because unicode
                /*
                char c0 = e.text.text[0];
                if (c0 == 'q') {
                    quit = true;
                }
                */
            } else if (e.type == SDL_KEYDOWN) {
                SDL_Keycode c = e.key.keysym.sym;
                if ((c == SDLK_ESCAPE) || (c == SDLK_q)) {
                    quit = true;
                } else if ((c == SDLK_F11) || (c == SDLK_f)) {
                    asserting_sdl(SDL_SetWindowFullscreen(
                                      window,
                                      is_fullscreen ? 0
                                      : SDL_WINDOW_FULLSCREEN_DESKTOP));
                    is_fullscreen = ! is_fullscreen;
                }
            }
        }

        struct timespec next_t = timespec_add(last_t, TIMESPEC_60th());
        if (need_sleep) {
            struct timespec t;
            GETTIME(&t);
            /* DBG_TIMESPEC(t); */
            struct timespec wait_t = timespec_sub(next_t, t);
            /* DBG_TIMESPEC(wait_t); */
            if (! timespec_is_zero(&wait_t)) {
                struct timespec rem_t;
                //redo:
                assert(0 == nanosleep(&wait_t, &rem_t));
                if (! timespec_is_zero(&rem_t)) {
                    /* DBG_TIMESPEC(rem_t); */
                    wait_t = rem_t;
                    //goto redo;
                }
            }
            last_t = next_t; // or t ?
        }
    }

    SDL_DestroyRenderer(renderer); //XX drop

    SDL_DestroyWindow(window);
    // SDL_Quit();
}

/// Set the drawing color that the `SDL_Renderer` should use for future
/// drawing.
static UNUSED
void set_draw_color(SDL_Renderer* renderer, Color color) {
    asserting_sdl(SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
                                      128 /* what is this? */));
}

/// Clear the rendering area, i.e. fill it with the current drawing
/// color.

/// Note that you can use the generic `clear` as a shortcut.

static UNUSED
void clear_SDL_Renderer(SDL_Renderer* renderer) {
    asserting_sdl(SDL_RenderClear(renderer));
}

/// Draw the given empty rectangle with the current colors.
static UNUSED
void draw_rect(SDL_Renderer* renderer, Rect2(float) r) {
    SDL_FRect sr = to_sdl(r);
    asserting_sdl(SDL_RenderDrawRectF(renderer, &sr));
}

/// Draw the given filled rectangle with the current colors.
static UNUSED
void draw_fill_rect(SDL_Renderer* renderer, Rect2(float) r) {
    SDL_FRect sr = to_sdl(r);
    asserting_sdl(SDL_RenderFillRectF(renderer, &sr));
}

/// Draw the given filled rectangles with the current colors.
static UNUSED
void draw_fill_rects(SDL_Renderer* renderer, slice(Rect2(float)) rects) {
    if (rects.len) {
        asserting_sdl(SDL_RenderFillRectsF(renderer,
                                           // pray
                                           (const SDL_FRect *)rects.ptr,
                                           rects.len));
    }
}


#include "sdlutil_circle.h"

/// Draw the given circle with the current colors.
static UNUSED
void draw_circle(SDL_Renderer* renderer, Vec2(int) pos, int radius) {
    asserting_sdl(SDL_RenderDrawCircle(renderer, pos.x, pos.y, radius));
}

/// Draw the given circle with the current colors.
static UNUSED
void draw_fill_circle(SDL_Renderer* renderer, Vec2(int) pos, int radius) {
    asserting_sdl(SDL_RenderFillCircle(renderer, pos.x, pos.y, radius));
}

/// Draw the given points.
static UNUSED
void draw_points_int(SDL_Renderer* renderer, slice(Vec2(int)) points) {
    if (points.len) {
        asserting_sdl(SDL_RenderDrawPoints(renderer,
                                           // pray
                                           (const SDL_Point *)points.ptr,
                                           points.len));
    }
}

/// Draw the given points (at subpixel precision).
static UNUSED
void draw_points_float(SDL_Renderer* renderer, slice(Vec2(float)) points) {
    if (points.len) {
        asserting_sdl(SDL_RenderDrawPointsF(renderer,
                                            // pray
                                            (const SDL_FPoint *)points.ptr,
                                            points.len));
    }
}

/// Draw a line from `from`, to `to` (at subpixel precision).
static UNUSED
void draw_line(SDL_Renderer *rdr, Vec2(float) from, Vec2(float) to) {
    asserting_sdl(SDL_RenderDrawLineF(rdr, from.x, from.y, to.x, to.y));
}

/// Draw a series of connected lines on the current rendering target
/// (at subpixel precision).
static UNUSED
void draw_lines(SDL_Renderer *renderer, slice(Vec2(float)) lines) {
    if (lines.len) {
        asserting_sdl(SDL_RenderDrawLinesF(renderer,
                                           // pray
                                           (const SDL_FPoint *)lines.ptr,
                                           lines.len));
    }
}


/// Original docs:

/// Returns the surface associated with the window.

/// A new surface will be created with the optimal format for the
/// window, if necessary. This surface will be freed when the window
/// is destroyed. Do not free this surface.

/// You may not combine this with 3D or the rendering API on this
/// window.

static UNUSED
SDL_Surface* get_Surface_from_Window(SDL_Window *window) {
    return asserting_sdl(SDL_GetWindowSurface(window));
}


// ------------------------------------------------------------------

/// Managing an owned SDL_Texture.

typedef struct Texture {
    SDL_Texture *ptr;
} Texture;

/// Create a Texture from an existing SDL_Texture pointer, taking
/// ownership.
#define Texture(sdltexture)                     \
    ((Texture) { .ptr = (sdltexture) })

static UNUSED
void drop_Texture(Texture self) {
    SDL_DestroyTexture(self.ptr);
}

static UNUSED
bool equal_Texture(const Texture *a, const Texture *b) {
    return a->ptr == b->ptr;
}

static UNUSED
int print_debug_Texture(const Texture *self) {
    INIT_RESRET;
    RESRET(printf("Texture(%p)", self->ptr));
cleanup:
    return ret;
}

GENERATE_Option(Texture);
GENERATE_ref(Texture);
GENERATE_Option(ref(Texture));

// ------------------------------------------------------------------
// Vertex renderer based drawing system.


/// Create a `SDL_Color`
#define ColorA(red, green, blue, strength)                      \
    ((SDL_Color) {                                              \
        .r = (red), .g = (green), .b = (blue), .a = (strength)  \
    })

static UNUSED
bool equal_SDL_Color(const SDL_Color *a, const SDL_Color *b) {
    return a->r == b->r
        && a->g == b->g
        && a->b == b->b
        && a->a == b->a;
}

static UNUSED
int print_debug_SDL_Color(const SDL_Color *self) {
    INIT_RESRET;
    RESRET(print_move_cstr("ColorA("));
    RESRET(print_debug_u8(&self->r));
    RESRET(print_move_cstr(", "));
    RESRET(print_debug_u8(&self->g));
    RESRET(print_move_cstr(", "));
    RESRET(print_debug_u8(&self->b));
    RESRET(print_move_cstr(", "));
    RESRET(print_debug_u8(&self->a));
    RESRET(print_move_cstr(")"));
cleanup:
    return ret;
}


// / Simplify the type name for our purposes.
// typedef SDL_Vertex Vertex;

/// A `Vertex` is a float-based position on the screen, color at that
/// point, and (optionally, can be set to {0, 0}) offset into a
/// texture, used for `VertexRenderer` based drawing.

/// It is memory-layout compatible with `SDL_Vertex`.

typedef struct Vertex {
    Vec2(float) position;
    SDL_Color color;
    Vec2(float) texture_position;
} Vertex;

static UNUSED
void drop_Vertex(UNUSED Vertex self) {}

static UNUSED
bool equal_Vertex(const Vertex *a, const Vertex *b) {
    return equal_Vec2_float(&a->position, &b->position)
        && equal_SDL_Color(&a->color, &b->color)
        && equal_Vec2_float(&a->texture_position, &b->texture_position);
}

static UNUSED
int print_debug_Vertex(const Vertex *self) {
    INIT_RESRET;
    RESRET(print_move_cstr("Vertex("));
    RESRET(print_debug_Vec2_float(&self->position));
    RESRET(print_move_cstr(", "));
    RESRET(print_debug_SDL_Color(&self->color));
    RESRET(print_move_cstr(", "));
    RESRET(print_debug_Vec2_float(&self->texture_position));
    RESRET(print_move_cstr(")"));
cleanup:
    return ret;
}

GENERATE_Option(Vertex);
GENERATE_ref(Vertex);
GENERATE_Option(ref(Vertex));

#define T Vertex
#include <cj50/gen/template/Vec.h>
#undef T


/// Constructor for a Vertex with full information.
static UNUSED
Vertex vertex_3(Vec2(float) position, SDL_Color color, Vec2(float) texture_position) {
    return (Vertex) { position, color, texture_position };
}

/// Constructor for a SDL_Vertex with texture position set to (0,0)
/// (e.g. for when not using textures).
static UNUSED
Vertex vertex_2(Vec2(float) position, SDL_Color color) {
    return (Vertex) { position, color, { 0, 0} };
}

/// A `VertexRenderer` collects vertices, and triples of indices into
/// the vertices vector representing triangles. The collected
/// triangles information can then be shown via the
/// `render_VertexRenderer` function.
typedef struct VertexRenderer {
    Vec(Vertex) vertices;
    Vec(Vec3(int)) indices;
} VertexRenderer;


/// Create a new `VertexRenderer`.
static UNUSED
VertexRenderer new_VertexRenderer() {
    assert(sizeof(Vertex) == sizeof(SDL_Vertex));
    return (VertexRenderer) {
        .vertices = new_Vec_Vertex(),
        .indices = new_Vec_Vec3_int(),
    };
}

/// Drop a `VertexRenderer`.
static UNUSED
void drop_VertexRenderer(VertexRenderer self) {
    drop_Vec_Vec3_int(self.indices);
    drop_Vec_Vertex(self.vertices);
}

static UNUSED
int print_debug_VertexRenderer(const VertexRenderer *self) {
    INIT_RESRET;
    RESRET(print_move_cstr("(VertexRenderer) {\n"));
    RESRET(print_move_cstr("  .vertices = "));
    RESRET(print_debug_Vec_Vertex(&self->vertices));
    RESRET(print_move_cstr("\n  .indices = "));
    RESRET(print_debug_Vec_Vec3_int(&self->indices));
    RESRET(print_move_cstr("\n}"));
cleanup:
    return ret;
}


/// Push one new vertex to the `VertexRenderer`, without registering
/// the vertex for rendering. Returns the index to the newly pushed
/// vertex; to register it for rendering, pass that index as part of a
/// triple of indices to `push_triangle`.

static UNUSED
int push_vertex(VertexRenderer *rdr, Vertex v) {
    push_Vec_Vertex(&rdr->vertices, v);
    size_t index = rdr->vertices.len - 1;
    assert(index <= INT_MAX);
    return index;
}

/// Push a triangle to the `VertexRenderer`, consisting of the indices
/// to vertices that were pushed before using `push_vertex`.

static UNUSED
void push_triangle(VertexRenderer *rdr, Vec3(int) indices) {
    push_Vec_Vec3_int(&rdr->indices, indices);
}

/// Render the `VertexRenderer` to the given `SDL_Renderer`. The
/// `VertexRenderer` is not consumed or cleared.
static UNUSED
void render_VertexRenderer(SDL_Renderer *renderer, VertexRenderer *rdr) {
    if (rdr->indices.len) {
        asserting_sdl(
            SDL_RenderGeometry(renderer,
                               NULL, // SDL_Texture *texture,
                               (SDL_Vertex *)rdr->vertices.ptr, rdr->vertices.len,
                               (int *)rdr->indices.ptr, rdr->indices.len * 3));
    }
}

/// Clear the `VertexRenderer`, so that it can be re-used.
static UNUSED
void clear_VertexRenderer(VertexRenderer *rdr) {
    clear_Vec_Vertex(&rdr->vertices);
    clear_Vec_Vec3_int(&rdr->indices);
}


// ------------------------------------------------------------------


/// Original docs:

/// Create a texture from an existing surface.

/// The surface is not modified or freed by this function.

/// The SDL_TextureAccess hint for the created texture is
/// SDL_TEXTUREACCESS_STATIC.

/// The pixel format of the created texture may be different from the
/// pixel format of the surface. Use SDL_QueryTexture() to query the
/// pixel format of the texture.

static UNUSED
Texture new_Texture_from_Surface(SDL_Renderer * renderer,
                                 SDL_Surface * surface) {
    return Texture(asserting_sdl(
                       SDL_CreateTextureFromSurface(renderer, surface)));
}

/// Create a texture for a rendering context.

/// format: 	one of the enumerated values in SDL_PixelFormatEnum

/// access: 	one of the enumerated values in SDL_TextureAccess

/// Aborts with an error message if no rendering context was active,
/// the format was unsupported, or the width or height were out of
/// range.

/// Also aborts if `dimensions` contains negative values.

static UNUSED
Texture create_Texture(SDL_Renderer * renderer,
                       Uint32 format,
                       int access,
                       Vec2(int) dimensions) {
    // XX: Are these checks good or does SDL work with negative values
    // (mirroring?)?
    assert(dimensions.x >= 0);
    assert(dimensions.y >= 0);
    return Texture(asserting_sdl(
                       SDL_CreateTexture(renderer,
                                         format,
                                         access,
                                         dimensions.x,
                                         dimensions.y)));
}

#define BORROW_from_Option_Rect2(T, rect2)       \
    rect2.is_some ? (T*)&rect2.value : NULL

/// Update the given texture rectangle with new pixel data.
/// 
/// rect: 	an SDL_Rect structure representing the area to update, or NULL to update the entire texture
/// pixels: 	the raw pixel data in the format of the texture
/// pitch: 	the number of bytes in a row of pixel data, including padding between lines

/// NOTE that pitch is in bytes, not pixels! This function is unsafe
/// because it has to handle various pixel formats.

/// (Todo: could it calculate pitch from metainformation, though?)

static UNUSED
void update_Texture(Texture *self,
                    Option(Rect2(int)) rect,
                    const void *pixels, // ugh
                    // why is pitch not in configuration? OK, pixels
                    // specific
                    int pitch) {
    asserting_sdl(SDL_UpdateTexture(
                      self->ptr,
                      BORROW_from_Option_Rect2(SDL_Rect, rect),
                      pixels,
                      pitch));
}

/// Copy a portion of the texture to the current rendering target.

static UNUSED
void render_Texture(SDL_Renderer *renderer,
                    Texture *texture,
                    Option(Rect2(int)) src,
                    Option(Rect2(int)) dst) {
    asserting_sdl(SDL_RenderCopy(renderer,
                                 texture->ptr,
                                 BORROW_from_Option_Rect2(SDL_Rect, src),
                                 BORROW_from_Option_Rect2(SDL_Rect, dst)));
}


// ------------------------------------------------------------------

static UNUSED
bool close_float(float a, float b) {
    return fabsf(a-b) < 0.1; // ~hack
}

static UNUSED
bool close_Vec2_float(const Vec2(float) *a, const Vec2(float) *b) {
    return close_float(a->x, b->x) && close_float(a->y, b->y);
}

static UNUSED
Vec2(float) turn_Vec2_float(Vec2(float) vec, float angle) {
    // XX faster? -- well, todo: calculate matrix anyway.
    float len = sqrtf(square(vec.x) + square(vec.y));
    float orig_angle = vec.y < 0.f ?
        asinf(vec.x / len) :
        math_pi_float - asinf(vec.x / len);
    float a = orig_angle + angle;
    AUTO res= vec2_float(sinf(a) * len,
                         - cosf(a) * len);
    /* assert(! ((angle == 0.f) && ! close_Vec2_float(&res, &vec))); */
    return res;
}

/// Draw the given ellipsoid with the given color onto the given
/// `VertexRenderer`, which is *not* cleared. The ellipsoid is positioned and
/// shaped so as to fit perfectly into `bounds`, before turning the result for
/// the given `turnangle` in radians (`0` .. `2 * math_pi`). `angle_from_to` are
/// the angles from which and to the circle should be drawn, if you want just a
/// slice (again in radians, in the shape before it is turned by
/// `turnangle`). `hole` determines how large of a hole is left in the center,
/// 0. meaning none, 1. meaning the hole is as large as the whole ellipsoid.
/// `num_segments` gives the number of segments used for a full circle/ellipsoid;
/// a value of about 20 is recommended (more segments may make drawing slower).

/// (Uses subpixel precision.)

/// See [examples/draw_circle.c](../examples/draw_circle.c) for an example.

static UNUSED
void draw_fill_ellipsoid(VertexRenderer* rdr,
                         Rect2(float) bounds,
                         Option(Vec2(float)) angle_from_to,
                         float hole, /* 0..1 */
                         float turnangle,
                         SDL_Color color,
                         u8 num_segments) {
    if (MIN(bounds.extent.x, bounds.extent.y) < 0.1)
        return;
    if (num_segments == 0)
        return;
    AUTO halfextent = mul(bounds.extent, 0.5);
    AUTO center = add(bounds.start, halfextent);
    int centerv = push_vertex(rdr, vertex_2(center, color));

    Vec2(float) angle_from_to_ =
        angle_from_to.is_some ? angle_from_to.value
        : vec2_float(0.f, 2.f * math_pi_float);

#define DIRVEC_AT_ANGLE(a)                                              \
    turn_Vec2_float(                                                    \
        vec2_float(sinf(a) * halfextent.x,                              \
                   - cosf(a) * halfextent.y),                           \
        turnangle)

    const AUTO topvec = DIRVEC_AT_ANGLE(angle_from_to_.x);
    const int topv = push_vertex(rdr, vertex_2(add(center, topvec), color));

    const bool has_hole = !(hole <= 0.f);

    const int topholev = has_hole
        ? push_vertex(rdr, vertex_2(add(center, mul(topvec, hole)), color))
        : centerv;

    int lastv = topv;
    int lastholev = topholev;
    const float d_angle = math_pi_float / num_segments;
    if (sdlutil_debug) {
        printf("num_segments=%i, d_angle=%f\n", num_segments, d_angle);
    }
    for (float i_angle = angle_from_to_.x + d_angle;
         i_angle < angle_from_to_.y;
         i_angle += d_angle)
    {
        AUTO vec = DIRVEC_AT_ANGLE(i_angle);
        int newv = push_vertex(rdr, vertex_2(add(center, vec), color));
        if (has_hole) {
            int newholev = push_vertex(rdr, vertex_2(add(center, mul(vec, hole)),
                                                     color));
            push_triangle(rdr, vec3_int(lastholev, lastv, newv));
            push_triangle(rdr, vec3_int(lastholev, newholev, newv));
            lastholev = newholev;
        } else {
            push_triangle(rdr, vec3_int(centerv, lastv, newv));
        }
        lastv = newv;
    }

    if ((angle_from_to_.y - angle_from_to_.x) >= 2.f * math_pi_float) {
        // last triangle(s) to close the circle
        if (has_hole) {
            push_triangle(rdr, vec3_int(lastholev, lastv, topv));
            push_triangle(rdr, vec3_int(lastholev, topholev, topv));
        } else {
            push_triangle(rdr, vec3_int(centerv, lastv, topv));
        }
    } else {
        // last triangle(s) to end of slice
        AUTO vec = DIRVEC_AT_ANGLE(angle_from_to_.y);
        int newv = push_vertex(rdr, vertex_2(add(center, vec), color));
        if (has_hole) {
            int newholev = push_vertex(rdr, vertex_2(add(center, mul(vec, hole)),
                                                     color));
            push_triangle(rdr, vec3_int(lastholev, lastv, newv));
            push_triangle(rdr, vec3_int(lastholev, newholev, newv));
        } else {
            push_triangle(rdr, vec3_int(centerv, lastv, newv));
        }
    }
#undef POINT_AT_ANGLE
}



#undef GETTIME
