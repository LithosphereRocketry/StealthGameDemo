#include "Tile.h"

TilePrototype::TilePrototype(std::string txpath, SDL_Rect* location, SDL_Rect* gbox) {
    sprite = std::make_unique<Sprite>(txpath, location);
    graphicsBox = *gbox;
}

int TilePrototype::draw(float camx, float camy, float zoom, SDL_Point* position) {
    SDL_Rect screenpos = {
        (int) round((position->x+graphicsBox.x-camx)*zoom) + screenCenter->x,
        (int) round((position->y+graphicsBox.y-camy)*zoom) + screenCenter->y,
        (int) round(graphicsBox.w*zoom),
        (int) round(graphicsBox.h*zoom)
    };
    return sprite->render(&screenpos);
}

int TilePrototype::drawClipped(float camx, float camy, float zoom, SDL_Point* position) {
    SDL_Rect screenpos = {
        (int) round((position->x+graphicsBox.x-camx)*zoom) + screenCenter->x,
        (int) round((position->y+graphicsBox.y-camy)*zoom) + screenCenter->y,
        (int) round(graphicsBox.w*zoom),
        (int) round(graphicsBox.h*zoom)
    };
    if(screenpos.x <= 2*screenCenter->x
    && screenpos.y <= 2*screenCenter->y
    && screenpos.x+screenpos.w >= 0
    && screenpos.y+screenpos.h >= 0) {
        return sprite->render(&screenpos);
    } else {
        return 0;
    }
}

std::unique_ptr<Tile> TilePrototype::instantiate(SDL_Point* loc) {
    return std::make_unique<Tile>(this, loc);
}