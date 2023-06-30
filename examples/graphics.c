#include <cj50.h>
#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(/* int argc, char* args[] */) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        DIE_("SDL could not initialize! SDL_Error: %s",
             SDL_GetError());
    }
    SDL_Window* window = SDL_CreateWindow(
        "SDL Tutorial",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
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

    
    float t = 0;
    
    SDL_Event e;
    bool quit = false;
    while (!quit) {

        t += 0.01;
        float factor = sin(t);

        assert_sdl(SDL_SetRenderDrawColor(renderer, 156,0,0, 128));
        assert_sdl(SDL_RenderClear(renderer));
        assert_sdl(SDL_SetRenderDrawColor(renderer, 0,156,0, 128));

        Vec2 diagonal = { 5, 5 };
        for (int i = 0; i < 70; i++) {
            Vec2 topleft = { 300 + i * factor * 4, i * 6 };
            Rect2 r = { topleft, diagonal };
            /* D(r); */
            SDL_Rect sr = sdl(r);
            assert_sdl(SDL_RenderDrawRect(renderer, &sr) < 0);
        }
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e)) {
            D(e.type);
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_TEXTINPUT) {
                D(e.text.text);
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

