#ifndef BASIC_TILES_H
#define BASIC_TILES_H

#include <string>

#include "VectorMath.h"
#include "Tile.h"
#include "Collision.h"

class PlainTile: public Tile, public NoneCollider {
    public:
        PlainTile(TilePrototype* proto, Vector<float, 2> pos);
};
class PlainTilePrototype: public TilePrototype {
    public:
        PlainTilePrototype(std::string txpath, SDL_Rect location,
                BoundingBox<float> bounds);
        virtual std::unique_ptr<Tile> instantiate(Vector<float, 2> loc);
};

class WallTile: public Tile, public CollisionBox {
    public:
        WallTile(TilePrototype* proto, Vector<float, 2> pos);
};
class WallTilePrototype: public TilePrototype {
    public:
        WallTilePrototype(std::string txpath, SDL_Rect location,
                BoundingBox<float> bounds);
        virtual std::unique_ptr<Tile> instantiate(Vector<float, 2> loc);
};

#endif