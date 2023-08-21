#pragma once

//! Utilities for working with the SDL2 library.

#include <SDL2/SDL.h>
#include <cj50/basic-util.h>
#include <cj50/CStr.h>
#include <cj50/math.h>
#include <cj50/instantiations/Vec_Vec2_int.h>
#include <cj50/instantiations/Vec_Vec2_float.h>
#include <cj50/instantiations/Vec2_u32.h>


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


/// Open a window with the given window size, and call `renderframe`
/// about 60 times per second to draw a new image each
/// time. `renderframe` is called with a `SDL_Renderer` that needs to
/// be passed to the drawing functions like `set_draw_color`, and the
/// `context` that is given to `graphics_render`; if you need to store
/// variables between subsequent frames, this is where you can put
/// them. If `renderframe` returns `true`, the image is drawn and
/// `renderframe` is called again 1/60 seconds later. If it returns
/// `false`, the drawing stops and `graphics_render` returns.

static UNUSED
void graphics_render(cstr title,
                     int window_width,
                     int window_height,
                     bool (*renderframe)(SDL_Renderer*, void*),
                     void* context)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        DIE_("SDL could not initialize! SDL_Error: %s",
             SDL_GetError());
    }
    SDL_Window* window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        window_width,
        window_height,
        SDL_WINDOW_SHOWN);
    if (!window) {
        DIE_("Window could not be created! SDL_Error: %s",
             SDL_GetError());
    }

    /*
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

    //Fill the surface white
    SDL_FillRect(screenSurface,
                 NULL,
                 SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
    
    //Update the surface
    SDL_UpdateWindowSurface(window);
    */
    SDL_Renderer * renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        DIE_("Accelerated renderer could not be created! SDL_Error: %s",
             SDL_GetError());
    }

    
    SDL_Event e;
    bool quit = false;
    while (!quit) {
        if (renderframe(renderer, context)) {
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
                }
            }
        }

        
        /* print("Outer loop.\n"); */
        /* sleep(1); */
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

/// Draw the given rectangle with the current colors.
static UNUSED
void draw_rect(SDL_Renderer* renderer, Rect2(float) r) {
    SDL_FRect sr = to_sdl(r);
    asserting_sdl(SDL_RenderDrawRectF(renderer, &sr));
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
    asserting_sdl(SDL_RenderDrawPoints(renderer,
                                    // pray
                                    (const SDL_Point *)points.ptr,
                                    points.len));
}

/// Draw the given points.
static UNUSED
void draw_points_float(SDL_Renderer* renderer, slice(Vec2(float)) points) {
    asserting_sdl(SDL_RenderDrawPointsF(renderer,
                                     // pray
                                     (const SDL_FPoint *)points.ptr,
                                     points.len));
}

/// Draw a line from `from`, to `to`.

static UNUSED
void draw_line(SDL_Renderer *rdr, Vec2(float) from, Vec2(float) to) {
    asserting_sdl(SDL_RenderDrawLineF(rdr, from.x, from.y, to.x, to.y));
}

/// Draw a series of connected lines on the current rendering target
/// at subpixel precision.

static UNUSED
void draw_lines(SDL_Renderer *renderer, slice(Vec2(float)) lines) {
    asserting_sdl(SDL_RenderDrawLinesF(renderer,
                                    // pray
                                    (const SDL_FPoint *)lines.ptr,
                                    lines.len));
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


typedef struct Texture {
    SDL_Texture *ptr;
} Texture;

static UNUSED
void drop_Texture(Texture self) {
    SDL_DestroyTexture(self.ptr);
}

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
    return (Texture) {
        .ptr = asserting_sdl(
            SDL_CreateTextureFromSurface(renderer, surface))
    };
}

/// Create a texture for a rendering context.

/// format: 	one of the enumerated values in SDL_PixelFormatEnum

/// access: 	one of the enumerated values in SDL_TextureAccess

/// Aborts with an error message if no rendering context was active,
/// the format was unsupported, or the width or height were out of
/// range.

static UNUSED
Texture create_Texture(SDL_Renderer * renderer,
                       Uint32 format,
                       int access,
                       Vec2(u32) dimensions) {
    return (Texture) {
        .ptr = asserting_sdl(
            SDL_CreateTexture(renderer,
                              format,
                              access,
                              dimensions.x,
                              dimensions.y))
    };
}

#define BORROW_from_Option_Rect2(T, rect2)       \
    rect2.is_some ? (T*)&rect2.value : NULL

/// Update the given texture rectangle with new pixel data.
/// 
/// rect: 	an SDL_Rect structure representing the area to update, or NULL to update the entire texture
/// pixels: 	the raw pixel data in the format of the texture
/// pitch: 	the number of bytes in a row of pixel data, including padding between lines

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

