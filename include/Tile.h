#ifndef TILE_H
#define TILE_H

#include <string>
#include <math.h>

#include <SDL2/SDL.h>

#include "Caching.h"
#include "Sprite.h"

inline void mergeBounds(const SDL_Rect* a, const SDL_Rect* b, SDL_Rect* dest) {
    dest->x = std::min(a->x, b->x);
    dest->y = std::min(a->y, b->y);
    dest->w = std::max(a->x+a->w, b->x+b->w) - dest->x;
    dest->h = std::max(a->y+a->h, b->y+b->h) - dest->y;
}
class TilePrototype;
// Class for things that have a "real-life" size and position
// Contains everything that is specific to just one tile
class Tile {
    public:
        TilePrototype* prototype;
        SDL_Point position;

        Tile() {}
        Tile(TilePrototype* proto, SDL_Point* pos) {
            prototype = proto;
            position = *pos;
        }
        // Draw regardless of position
        int draw(float camx, float camy, float zoom);
        // Draw only if onscreen
        int drawClipped(float camx, float camy, float zoom);
};
// Basically a sprite manager for tiles
// Contains everything that is common between all tiles of a type
class TilePrototype {
    public:
        SDL_Point* screenCenter;
        SDL_Rect graphicsBox;
        TilePrototype(std::string txpath, SDL_Rect* location, SDL_Rect* gbox);
        ~TilePrototype();
        void load(CachedRenderer* dest) {
            sprite->load(dest);
            screenCenter = &dest->center;
        }
        int draw(float camx, float camy, float zoom, SDL_Point* position);
        int drawClipped(float camx, float camy, float zoom, SDL_Point* position);
        Tile* instantiate(SDL_Point* loc);
    private:
        Sprite* sprite;
};

// Class for tile grids
// This class "owns" all of its tiles
class TileGrid {
    public:
        TileGrid(size_t w, size_t h, int tw, int th) {
            shape.w = w;
            shape.h = h;
            tileWidth = tw;
            tileHeight = th;
            buffer = new Tile*[shape.w*shape.h](); // () needed here to make sure the tiles get initialized as nullptr
        }
        ~TileGrid() {
            delete[] buffer;
        }
        Tile** operator [] (size_t i) {
            return buffer + shape.w*i;
        }
        inline void fill(TilePrototype* prototype) {
            fillRect(prototype, 0, 0, shape.w, shape.h);
        }
        inline void put(TilePrototype* prototype, int x, int y) {
            if(operator[](y)[x]) {
                delete operator[](y)[x];
            }
            SDL_Point p = {x*tileWidth, y*tileHeight};
            Tile* t = new Tile(prototype, &p);
            mergeBounds(&prototype->graphicsBox, &maxBound, &maxBound);
            operator[](y)[x] = t;
        }
        void fillRect(TilePrototype* prototype, int x, int y, int w, int h) {
            for(int i = y; i < y+h; i++) {
                for(int j = x; j < x+w; j++) {
                    put(prototype, j, i);
                }
            }
        }
        void draw(float camx, float camy, float zoom) {
            SDL_Point* ctr = NULL;
            for(int i = 0; i < shape.w*shape.h; i++) {
                if(buffer[i] && buffer[i]->prototype && buffer[i]->prototype->screenCenter) {
                    ctr = buffer[i]->prototype->screenCenter;
                    break;
                }
            }
            if(!ctr) { return; }
            int xmin = std::max((float) 0, (-maxBound.x - maxBound.w - ctr->x/zoom)/tileWidth);
            int ymin = std::max((float) 0, (-maxBound.y - maxBound.h - ctr->y/zoom)/tileHeight);
            int xmax = std::min((float) shape.w, (-maxBound.x + ctr->x/zoom)/tileWidth + 1);
            int ymax = std::min((float) shape.h, (-maxBound.y + ctr->y/zoom)/tileHeight + 1);
            for(int i = ymin; i < ymax; i++) {
                for(int j = xmin; j < xmax; j++) {
                    if(!buffer[i*shape.w + j]) {
                        std::cerr << "Warning: tried to draw an uninitialized tile\n";
                    }
                    buffer[i*shape.w + j]->draw(camx, camy, zoom);
                }
            }
        }
    private:
        SDL_Rect shape = {0, 0, 0, 0};
        int tileWidth;
        int tileHeight;
        Tile** buffer;
        SDL_Rect maxBound = {0, 0, 0, 0};
};

#endif