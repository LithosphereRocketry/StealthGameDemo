#ifndef TILE_H
#define TILE_H

#include <string>
#include <math.h>

#include <SDL2/SDL.h>

#include "Sprite.h"

class Tile {
    public:
        SDL_Point position;
        SDL_Rect graphicsBox;

        Tile(std::string texturepath, SDL_Rect* location) {
            txpath = texturepath;
            if(location) {
                sheetpos = new SDL_Rect;
                *sheetpos = *location;
            }
        }
        ~Tile() {
            delete sprite;
            delete sheetpos;
        }
        void loadGraphics(CachedRenderer* dest) {
            sprite = new Sprite(dest, txpath, sheetpos);
        }
        void draw(float camx, float camy, float zoom) {
            if(sprite) {
                SDL_Rect pos = { // +0.5 == round
                    (int) round((position.x+graphicsBox.x-camx)*zoom),
                    (int) round((position.y+graphicsBox.y-camy)*zoom),
                    (int) round(graphicsBox.w*zoom),
                    (int) round(graphicsBox.h*zoom)
                };
                sprite->render(&pos);
            }
        }
    private:
        std::string txpath;
        Sprite* sprite = NULL;
        SDL_Rect* sheetpos = NULL;
};

#endif