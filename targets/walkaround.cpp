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
    Camera cam {&cr, {16, 12}, 20, 20};
    cout << "Done.\n";

    TileGrid grid(32, 24, 1, 1);
    grid.fill(&wall);
    grid.fillRect(&floor, 1, 1, 30, 22);
    grid.fillRect(&wall, 10, 10, 12, 4);
    grid.load(&cam);

    GameObject test(1, 0.5, 16, 6, {0.3, 0.3, 0.6}, 100, 5, &grid);

    bool quit = false;

    bool wup = false;
    bool wdn = false;
    bool wlf = false;
    bool wrt = false;
    while(!quit) {
        auto start = chrono::steady_clock::now();

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        // for some reason clearing the screen is a lot faster
        // it really feels like there should be a way to not clear but leave the
        // framebuffer untouched but hey this is what the internet recommends
        grid.draw();

        Vector<float, 2> offset = Vector<float, 2>({16, 6}) - test.pos;
        if(grid.getFreePath(test.pos, offset, test.radius).type == FREE) {
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
        } else if(grid.getFreePath(test.pos, offset, 0).type == FREE) {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);
        } else {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        }
        drawVector(&cam, offset, test.pos);
        drawPoint(&cam, test.pos, test.radius);

        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_KEYUP:
                    switch(e.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                            wup = false; break;
                        case SDL_SCANCODE_S:
                            wdn = false; break;
                        case SDL_SCANCODE_A:
                            wlf = false; break;
                        case SDL_SCANCODE_D:
                            wrt = false; break;
                        default: break;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch(e.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                            wup = true; break;
                        case SDL_SCANCODE_S:
                            wdn = true; break;
                        case SDL_SCANCODE_A:
                            wlf = true; break;
                        case SDL_SCANCODE_D:
                            wrt = true; break;
                        default: break;
                    }
                    break;
                case SDL_QUIT:
                    quit = true;
                    break;
                // no default as an unrecognized event basically doesn't matter
            }
        }
        
        float walkup = (wup ? 1 : 0) + (wdn ? -1 : 0);
        float walkrt = (wrt ? 1 : 0) + (wlf ? -1 : 0);
        Vector<float, 2> walk = {walkrt, walkup};
        test.applyForce(walk.toMag(150));
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

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}