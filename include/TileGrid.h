#ifndef TILEGRID_H
#define TILEGRID_H

#include "Tile.h"
#include "VectorMath.h"

// Class for tile grids
// This class "owns" all of its tiles
class TileGrid {
    public:
        TileGrid(int w, int h, int tw, int th);
        inline Tile* index(int x, int y) { return buffer[y*shape.w + x].get(); }
        inline void fill(TilePrototype* prototype) {
            fillRect(prototype, 0, 0, shape.w, shape.h);
        }
        void put(TilePrototype* prototype, int x, int y);
        void fillRect(TilePrototype* prototype, int x, int y, int w, int h);
        void load(Camera* cam);
        void draw();
    private:
        SDL_Rect shape;
        float tileWidth, tileHeight;
        std::vector<std::unique_ptr<Tile>> buffer;
        BoundingBox<float> maxBound;
        Camera* activeCam;
};

#endif