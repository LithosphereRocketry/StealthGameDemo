#include <iostream>
#include <chrono>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Caching.h"
#include "Tile.h"
#include "Collision.h"
#include "SDLDebug.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int samplect = 1000;
int samples;
long int tottime;

EdgeCollider edge({0, -5}, {1, 10});
EdgeCollider wall({5, -5}, {-3, 1});
SegmentCollider segment({-1, -2}, {2, 1});
CollisionGroup grp;
CollidingObject test(0, 10, 1, 1, {0.9, 1}, &grp);

int main(int argc, char** argv) {

    grp.colliders.push_back(&edge);
    grp.colliders.push_back(&wall);
    grp.colliders.push_back(&segment);

    SDL_Window* window = nullptr;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL not initialized: error " << SDL_GetError() << endl;
        return -1;
    }

    window = SDL_CreateWindow( "Hello SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL ) {
        cout << "SDL window creation failed: error " << SDL_GetError() << endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    CachedRenderer cr = CachedRenderer(renderer);

    SDL_Rect wallbound = {16, 0, 16, 32};
    SDL_Rect floorbound = {64, 32, 16, 32};
    SDL_Rect tilebounds = {0, -100, 100, 200};

    TilePrototype wall("tilesheet.png", &wallbound, &tilebounds);
    wall.load(&cr);

    TilePrototype floor("tilesheet.png", &floorbound, &tilebounds);
    floor.load(&cr);

    TileGrid walls = TileGrid(1000, 1000, 100, 100);
    walls.fill(&wall);
    walls.fillRect(&floor, 1, 1, 8, 8);

    bool quit = false;
    while(!quit) {
        auto start = chrono::steady_clock::now();

        //SDL_RenderClear(renderer); // for some reason clearing the screen is a lot faster
                                     // it really feels like there should be a way to not clear but leave the framebuffer untouched
        //walls.draw(0, 0, 0.25);
        drawPoint(renderer, test.pos, {0, 0}, 20, test.radius);
        
        SDL_RenderDrawLine(renderer, 320 + 20*edge.position[0], 240 - 20*edge.position[1],
            320 + 20*edge.position[0] + 20*edge.normal[0], 240 - 20*edge.position[1] - 20*edge.normal[1]);
        
        test.applyForce({0, -1});

        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_KEYUP:
                    break;
                case SDL_KEYDOWN:
                    test.step(0.05);
                    break;
                case SDL_QUIT:
                    quit = true;
                    break;
                // no default as an unrecognized event basically doesn't matter
            }
        }
        
        

        cr.display();
        bool idling = false;
        while(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count() < 50) {
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