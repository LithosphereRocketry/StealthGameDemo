#include <chrono>

#define SDL_MAIN_HANDLED
#include "BasicTiles.h"
#include "TileGrid.h"
#include "SDLDebug.h"
#include "GameObject.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
#define TARGET_FRAMERATE 100

using namespace std;

int main() {
    SDL_Rect wallbound = {64, 48, 16, 16};
    SDL_Rect floorbound = {0, 16, 16, 16};
    BoundingBox<float> tilebounds({0, 0}, {1, 1});
    WallTilePrototype wall("tilesheet.png", wallbound, tilebounds);
    PlainTilePrototype floor("tilesheet.png", floorbound, tilebounds);

    cout << "Initializing video...";
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL not initialized: error " << SDL_GetError() << endl;
        return -1;
    }
    SDL_Window* window = SDL_CreateWindow( "Hello SDL", SDL_WINDOWPOS_UNDEFINED,
                                            SDL_WINDOWPOS_UNDEFINED,
                                            SCREEN_WIDTH, SCREEN_HEIGHT,
                                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
    if(!window) {
        cout << "SDL window creation failed: error " << SDL_GetError() << endl;
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    cout << "Done.\n";

    cout << "Initializing camera...";
    CachedRenderer cr(renderer);
    int x = 5;
    Camera cam {&cr, {6, x-1}, 20, 20};
    cout << "Done.\n";

    TileGrid grid(1, x+1, 1, 1);
    grid.fill(&floor);
    grid.fillRect(&wall, 0, x, 1, 1);
    grid.load(&cam);

    GameObject test(1, 0.5, -0.25, x-7, {0.3, 0.3, 0.6}, 100, 5, &grid);

    bool quit = false;

    bool wup = false;
    bool wdn = false;
    bool wlf = false;
    bool wrt = false;
    while(!quit) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        grid.draw();
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
        drawPoint(&cam, test.pos, test.radius);

        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                // no default as an unrecognized event basically doesn't matter
            }
        }
        
        test.applyForce({0, 150});
        test.step(1.0f/TARGET_FRAMERATE);

        cr.display();
        if(test.pos[1] > 10) {
            quit = true;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}