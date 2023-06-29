#ifndef SDLUTIL_H_
#define SDLUTIL_H_

#include <SDL2/SDL.h>
#include "u8l/basic-util.h"

void assert_sdl_int(int code) {
    if (code < 0) {
        DIE_("SDL error: %s", SDL_GetError());
    }
}


#define assert_sdl(v)                           \
    _Generic((v)                                \
             , int: assert_sdl_int              \
        )(v)




#endif /* SDLUTIL_H_ */
