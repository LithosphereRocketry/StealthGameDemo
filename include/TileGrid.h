#ifndef TILEGRID_H
#define TILEGRID_H

#include "Tile.h"
#include "VectorMath.h"
#include "Collision.h"

// Class for tile grids
// This class "owns" all of its tiles
class TileGrid: public Collidable {
    public:
        TileGrid(int w, int h, float tw, float th);
        inline Tile* index(int x, int y) { return buffer[y*shape.w + x].get(); }
        inline void fill(TilePrototype* prototype) {
            fillRect(prototype, 0, 0, shape.w, shape.h);
        }
        void put(TilePrototype* prototype, int x, int y);
        void fillRect(TilePrototype* prototype, int x, int y, int w, int h);
        void load(Camera* cam);
        void draw();

        virtual FreePathResult getFreePath(const Ray2f& path, float radius, float bound);
    private:
        SDL_Rect shape;
        float tileWidth, tileHeight;
        std::vector<std::unique_ptr<Tile>> buffer;
        BoundingBox<float> drawBound;
        BoundingBox<float> collisionBound;
        Camera* activeCam;
};

#endif