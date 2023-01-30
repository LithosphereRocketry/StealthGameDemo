#include "Tile.h"

int Tile::draw(float camx, float camy, float zoom) {
    if(prototype) {            
        return prototype->draw(camx, camy, zoom, &position);
    } else {
        return 0;
    }
}

int Tile::drawClipped(float camx, float camy, float zoom) {
    if(prototype) {            
        return prototype->drawClipped(camx, camy, zoom, &position);
    } else {
        return 0;
    }
}