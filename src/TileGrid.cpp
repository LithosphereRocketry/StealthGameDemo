#include "Tile.h"

TileGrid::TileGrid(size_t w, size_t h, int tw, int th) {
    shape.w = w;
    shape.h = h;
    tileWidth = tw;
    tileHeight = th;
    buffer = std::vector<std::unique_ptr<Tile>>(w*h);
}

void TileGrid::put(TilePrototype* prototype, int x, int y) {
    SDL_Point p = {x*tileWidth, y*tileHeight};
    mergeBounds(&prototype->graphicsBox, &maxBound, &maxBound);
    buffer[y*shape.w + x] = std::unique_ptr<Tile>(std::move(prototype->instantiate(&p)));
}

void TileGrid::fillRect(TilePrototype* prototype, int x, int y, int w, int h) {
    for(int i = y; i < y+h; i++) {
        for(int j = x; j < x+w; j++) {
            put(prototype, j, i);
        }
    }
}

void TileGrid::draw(float camx, float camy, float zoom) {
    SDL_Point* ctr = NULL;
    for(int i = 0; i < shape.w*shape.h; i++) {
        if(buffer[i] && buffer[i]->prototype && buffer[i]->prototype->screenCenter) {
            ctr = buffer[i]->prototype->screenCenter;
            break;
        }
    }
    if(!ctr) { return; }
    int xmin = std::max((float) 0, (-maxBound.x - maxBound.w - ctr->x/zoom)/tileWidth);
    int ymin = std::max((float) 0, (-maxBound.y - maxBound.h - ctr->y/zoom)/tileHeight);
    int xmax = std::min((float) shape.w, (-maxBound.x + ctr->x/zoom)/tileWidth + 1);
    int ymax = std::min((float) shape.h, (-maxBound.y + ctr->y/zoom)/tileHeight + 1);
    for(int i = ymin; i < ymax; i++) {
        for(int j = xmin; j < xmax; j++) {
            if(index(j, i)) {
                index(j, i)->draw(camx, camy, zoom);
            } else {
                std::cerr << "Warning: tried to draw an uninitialized tile at (" << j << ", " << i << ")\n";
            }
        }
    }
}