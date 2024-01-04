#ifndef BASIC_TILES_H
#define BASIC_TILES_H

#include <string>

#include "VectorMath.h"
#include "Tile.h"
#include "Collision.h"

class PlainTile: public Tile, public NoneCollider {
    public:
        PlainTile(TilePrototype* proto, Vec2f pos);
};
class PlainTilePrototype: public TilePrototype {
    public:
        PlainTilePrototype(std::string txpath, SDL_Rect location,
                BoundingBox<float> bounds);
        virtual std::unique_ptr<Tile> instantiate(Vec2f loc);
};

class WallTile: public Tile, public CollisionBox {
    public:
        WallTile(TilePrototype* proto, Vec2f pos);
};
class WallTilePrototype: public TilePrototype {
    public:
        WallTilePrototype(std::string txpath, SDL_Rect location,
                BoundingBox<float> bounds);
        virtual std::unique_ptr<Tile> instantiate(Vec2f loc);
};

#endif