#ifndef SDLUTIL_H_
#define SDLUTIL_H_

//! Utilities for working with the SDL2 library.

#include <SDL2/SDL.h>
#include "u8l/basic-util.h"

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




#endif /* SDLUTIL_H_ */
