#include <cj50.h>
#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(/* int argc, char* args[] */) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        ABORT("SDL could not initialize! SDL_Error: %s\n",
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
        ABORT("Window could not be created! SDL_Error: %s\n",
              SDL_GetError());
    }
    
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

    //Fill the surface white
    SDL_FillRect(screenSurface,
                 NULL,
                 SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
    
    //Update the surface
    SDL_UpdateWindowSurface(window);

    SDL_Renderer * renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        ABORT("Accelerated renderer could not be created! SDL_Error: %s\n",
              SDL_GetError());
    }
    assert_sdl(SDL_SetRenderDrawColor(renderer, 156,0,0,128));
    SDL_Rect r = { 10, 20, 30, 40 };
    assert_sdl(SDL_RenderDrawRect(renderer, &r) < 0);

    //Hack to get window to stay up
    SDL_Event e;
    bool quit = false;
    while (!quit) {
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

