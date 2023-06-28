#ifndef SDLUTIL_H_
#define SDLUTIL_H_

#include <SDL2/SDL.h>
#include "util.h"

void assert_sdl_int(int code) {
    if (code < 0) {
        ABORT("SDL error: %s\n", SDL_GetError());
    }
}


#define assert_sdl(v)                           \
    _Generic((v)                                \
             , int: assert_sdl_int              \
        )(v)




#endif /* SDLUTIL_H_ */
