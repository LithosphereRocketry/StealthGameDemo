#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>

#include "Caching.h"
#include "Tile.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char** argv) {
    SDL_Window* window = nullptr;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL not initialized: error " << SDL_GetError() << endl;
        return -1;
    }

    window = SDL_CreateWindow( "Hello SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL ) {
        cout << "SDL window creation failed: error " << SDL_GetError() << endl;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);

    CachedRenderer cr = CachedRenderer(renderer);

    SDL_Rect wallbound = {16, 0, 16, 32};
    SDL_Rect floorbound = {64, 32, 16, 32};

    Tile wall = Tile("tilesheet.png", &wallbound);
    wall.graphicsBox = {0, -100, 100, 200};
    wall.load(&cr);

    Tile floor = Tile("tilesheet.png", &floorbound);
    floor.graphicsBox = {0, -100, 100, 200};
    floor.load(&cr);

    TileGrid walls = TileGrid(10, 10, 100, 100);
    walls.fill(&wall);
    walls.fillRect(&floor, 1, 1, 8, 8);

    bool quit = false;
    while(quit == false) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }
        }
        SDL_RenderClear(renderer);
        walls.draw(0, 0, 0.25);
        cr.display();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}