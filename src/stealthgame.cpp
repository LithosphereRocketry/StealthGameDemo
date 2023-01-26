#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

using namespace std;


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char** argv) {
    SDL_Window* window;
    SDL_Surface* surface;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL not initialized: error " << SDL_GetError() << endl;
        return -1;
    }

    window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL ) {
        cout << "SDL window creation failed: error " << SDL_GetError() << endl;
    }

    surface = SDL_GetWindowSurface( window );
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
    
    SDL_UpdateWindowSurface( window );

    SDL_Event e;
    bool quit = false;
    while( quit == false ) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT)
                quit = true;
        }
    }

    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}