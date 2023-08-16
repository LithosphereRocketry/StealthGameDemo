#include <chrono>
#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "TileGrid.h"
#include "BasicTiles.h"
#include "Caching.h"
#include "SDLDebug.h"

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 400;

#define TARGET_FRAMERATE 100

using namespace std;

int main(int argc, char** argv) {
    SDL_Rect wallbound = {64, 48, 16, 16};
    SDL_Rect floorbound = {0, 16, 16, 16};
    BoundingBox<float> tilebounds {{0, 0}, {1, 1}};

    WallTilePrototype wall("tilesheet.png", wallbound, tilebounds);
    PlainTilePrototype floor("tilesheet.png", floorbound, tilebounds);

    cout << "start\n";

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL not initialized: error " << SDL_GetError() << endl;
        return -1;
    }
    SDL_Window* window = SDL_CreateWindow( "Hello SDL", SDL_WINDOWPOS_UNDEFINED,
                                            SDL_WINDOWPOS_UNDEFINED,
                                            SCREEN_WIDTH, SCREEN_HEIGHT,
                                            SDL_WINDOW_SHOWN );
    if(!window) {
        cout << "SDL window creation failed: error " << SDL_GetError() << endl;
        return -1;
    }

    cout << "window\n";
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED);
    CachedRenderer cr(renderer);
    Camera cam {&cr, {6, 4}, 50, 50};
    cout << "rendering\n";
    TileGrid walls = TileGrid(12, 8, 1, 1);
    walls.fill(&wall);
    walls.fillRect(&floor, 1, 1, 10, 6);
    walls.load(&cam);
    cout << "walls\n";

    CollidingObject test(3, 3, 1, 0.5, {1, 1, 0}, &walls);
    test.applyDV({10, 10});

    bool quit = false;
    while(!quit) {
        auto start = chrono::steady_clock::now();

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        walls.draw();

        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
        drawPoint(&cam, test.pos, test.radius);
        drawBBox(&cam, walls.index(6, 0)->getBounds());

        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }
        
        test.applyForce({0, -9.8});
        test.step(1.0f/TARGET_FRAMERATE);
        cr.display();
        bool idling = false;
        while(chrono::duration_cast<chrono::milliseconds>
                (chrono::steady_clock::now() - start).count()
                 < 1000/TARGET_FRAMERATE) {
            idling = true;
            SDL_Delay(1);
        }
        if(!idling) {
            cout << "Warning: frames dropped\n";
        }
    }
}