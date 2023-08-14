#include "Tile.h"
#include "SDLDebug.h"

TilePrototype::TilePrototype(std::string txpath, SDL_Rect location, BoundingBox<float> bounds) {
    sprite = std::make_unique<WorldSprite>(txpath, location, bounds);
}

// TODO: reimplement if this turns out to be good; right now it barely matters
// int TilePrototype::drawClipped(Vector<float, 2> pos) {
//     SDL_Rect screenpos = {
//         (int) round((position->x+graphicsBox.x-camx)*zoom) + screenCenter->x,
//         (int) round((position->y+graphicsBox.y-camy)*zoom) + screenCenter->y,
//         (int) round(graphicsBox.w*zoom),
//         (int) round(graphicsBox.h*zoom)
//     };
//     if(screenpos.x <= 2*screenCenter->x
//     && screenpos.y <= 2*screenCenter->y
//     && screenpos.x+screenpos.w >= 0
//     && screenpos.y+screenpos.h >= 0) {
//         return sprite->render(&screenpos);
//     } else {
//         return 0;
//     }
// }

// TODO: this can't exist because Tile is abstract now
// std::unique_ptr<Tile> TilePrototype::instantiate(Vector<float, 2> loc) {
//     return std::make_unique<Tile>(this, loc);
// }

BoundingBox<float> TilePrototype::getBounds() {
    return sprite->bbox;
}