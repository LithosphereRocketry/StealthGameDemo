#ifndef TILE_H
#define TILE_H

#include <string>
#include <vector>
#include <math.h>
#include <memory>

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
        void load(CachedRenderer* dest) {
            sprite->load(dest);
            screenCenter = &dest->center;
        }
        int draw(float camx, float camy, float zoom, SDL_Point* pos);
        int drawClipped(float camx, float camy, float zoom, SDL_Point* pos);
        std::unique_ptr<Tile> instantiate(SDL_Point* loc);
    private:
        std::unique_ptr<Sprite> sprite;
};

// Class for tile grids
// This class "owns" all of its tiles
class TileGrid {
    public:
        TileGrid(size_t w, size_t h, int tw, int th);
        inline Tile* index(int x, int y) { return buffer[y*shape.w + x].get(); }
        inline void fill(TilePrototype* prototype) {
            fillRect(prototype, 0, 0, shape.w, shape.h);
        }
        void put(TilePrototype* prototype, int x, int y);
        void fillRect(TilePrototype* prototype, int x, int y, int w, int h);
        void draw(float camx, float camy, float zoom);
    private:
        SDL_Rect shape = {0, 0, 0, 0};
        int tileWidth;
        int tileHeight;
        std::vector<std::unique_ptr<Tile>> buffer;
        SDL_Rect maxBound = {0, 0, 0, 0};
};

#endif