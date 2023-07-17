#ifndef TILE_H
#define TILE_H

#include <string>
#include <vector>
#include <math.h>
#include <memory>

#include <SDL2/SDL.h>

#include "Caching.h"
#include "WorldSprite.h"
#include "VectorMath.h"
#include "BoundingBox.h"

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
        Vector<float, 2> position;

        Tile() {}
        Tile(TilePrototype* proto, Vector<float, 2> pos) {
            prototype = proto;
            position = pos;
        }
        // Draw regardless of position
        int draw();
        // Draw only if onscreen (temporarily removed)
        // int drawClipped();
};
// Basically a sprite manager for tiles
// Contains everything that is common between all tiles of a type
class TilePrototype {
    public:
        TilePrototype(std::string txpath, SDL_Rect location, BoundingBox<float> bounds);
        void load(Camera* dest) { sprite->load(dest); }
        int draw(Vector<float, 2> pos) { return sprite->render(pos); }
        // int drawClipped(Vector<float, 2> pos);
        std::unique_ptr<Tile> instantiate(Vector<float, 2> loc); // TODO: should this return a RVR?
        BoundingBox<float> getBounds();
    private:
        std::unique_ptr<WorldSprite> sprite; // TODO: could this just be an owned object? Is WorldSprite ever polymorphic?
};

#endif