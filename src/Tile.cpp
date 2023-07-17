#include "Tile.h"

int Tile::draw() {
    if(prototype) {            
        return prototype->draw(position);
    } else {
        return 0;
    }
}

// TODO: see TilePrototype::drawClipped()
// int Tile::drawClipped() {
//     if(prototype) {            
//         return prototype->drawClipped(position);
//     } else {
//         return 0;
//     }
// } 