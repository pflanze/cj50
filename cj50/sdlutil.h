#pragma once

//! Utilities for working with the SDL2 library.

#include <SDL2/SDL.h>
#include <cj50/basic-util.h>

int assert_sdl_int(int code) {
    if (code < 0) {
        DIE_("SDL error: %s", SDL_GetError());
    } else {
        return code;
    }
}


/// Assert that the return value of a function from the SDL2 libray
/// does not represent an error. If it is an error, print the SDL
/// error message and abort. Otherwise, return the value.

#define assert_sdl(v)                           \
    _Generic((v)                                \
             , int: assert_sdl_int              \
        )(v)


void graphics_render(int screen_width,
                     int screen_height,
                     bool (*renderframe)(SDL_Renderer*, void*),
                     void* context)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        DIE_("SDL could not initialize! SDL_Error: %s",
             SDL_GetError());
    }
    SDL_Window* window = SDL_CreateWindow(
        "SDL Tutorial",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        screen_width,
        screen_height,
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
            /* D(e.type); */
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_TEXTINPUT) {
                /* D(e.text.text); */
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
    SDL_Quit();
}

/// Set the drawing color that the SDL_Renderer should use for future
/// drawing.
void set_draw_color(SDL_Renderer* renderer, Color color) {
    assert_sdl(SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
                                      128 /* what is this? */));
}

/// Clear the rendering area, i.e. fill it with the current drawing
/// color.
void clear(SDL_Renderer* renderer) {
    assert_sdl(SDL_RenderClear(renderer));
}

/// Draw the given rectangle with the current colors.
void draw_rect(SDL_Renderer* renderer, Rect2 r) {
    SDL_Rect sr = to_sdl(r);
    assert_sdl(SDL_RenderDrawRect(renderer, &sr));
}

