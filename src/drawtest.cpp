#include <iostream>

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "Caching.h"
#include "Tile.h"
#include "TileGrid.h"

using namespace std;

int main() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL not initialized: error " << SDL_GetError() << endl;
        return -1;
    }
    SDL_Window* window = SDL_CreateWindow( "Hello SDL", SDL_WINDOWPOS_UNDEFINED,
                                            SDL_WINDOWPOS_UNDEFINED,
                                            640, 480, SDL_WINDOW_SHOWN );
    if(!window) {
        cout << "SDL window creation failed: error " << SDL_GetError() << endl;
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    CachedRenderer cr(renderer);
    Camera cam {&cr, {0, 0}, 100, 100};
    
    TilePrototype t("tilesheet.png", {0, 0, 20, 20}, {{0, 0}, {1, 1}});

    TileGrid walls = TileGrid(1000, 1000, 1, 1);    
    walls.fill(&t);
    walls.load(&cam);
    walls.draw();
    cr.display();
    bool quit = false;
    while(!quit) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                // no default as an unrecognized event basically doesn't matter
            }
        }
        SDL_Delay(1);
    }
}