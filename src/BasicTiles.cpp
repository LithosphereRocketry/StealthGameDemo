#include "BasicTiles.h"



// Plain tiles

PlainTile::PlainTile(TilePrototype* proto, Vector<float, 2> pos):
        Tile(proto, pos), NoneCollider() {}

PlainTilePrototype::PlainTilePrototype(std::string txpath, SDL_Rect location,
        BoundingBox<float> bounds): TilePrototype(txpath, location, bounds) {}

std::unique_ptr<Tile> PlainTilePrototype::instantiate(Vector<float, 2> loc) {
    return std::make_unique<PlainTile>(this, loc);
}

// Wall tiles

WallTile::WallTile(TilePrototype* proto, Vector<float, 2> pos):
        Tile(proto, pos), CollisionBox(getBounds()) {}

WallTilePrototype::WallTilePrototype(std::string txpath, SDL_Rect location,
        BoundingBox<float> bounds): TilePrototype(txpath, location, bounds) {}

std::unique_ptr<Tile> WallTilePrototype::instantiate(Vector<float, 2> loc) {
    return std::make_unique<WallTile>(this, loc);
}