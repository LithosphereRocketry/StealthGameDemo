#include "TileGrid.h"

TileGrid::TileGrid(int w, int h, int tw, int th):
        shape({0, 0, w, h}), tileWidth(tw), tileHeight(th), buffer(w*h),
        maxBound{{0, 0}, {0, 0}}, activeCam(nullptr) {
}

void TileGrid::put(TilePrototype* prototype, int x, int y) {
    Vector<float, 2> p = {x*tileWidth, y*tileHeight};
    maxBound |= prototype->getBounds();
    prototype->load(activeCam);
    buffer[y*shape.w + x] = std::unique_ptr<Tile>(prototype->instantiate(p));
}

void TileGrid::fillRect(TilePrototype* prototype, int x, int y, int w, int h) {
    for(int i = y; i < y+h; i++) {
        for(int j = x; j < x+w; j++) {
            put(prototype, j, i);
        }
    }
}

void TileGrid::load(Camera* newcam) {
    activeCam = newcam;
    for(auto const& t : buffer) {
        t->prototype->load(newcam);
    }
}

void TileGrid::draw() {
    BoundingBox<float> spaceBounds = activeCam->visibleBounds();
    int xmin = std::max(0, int(std::floor(spaceBounds.c1[0])));
    int ymin = std::max(0, int(std::floor(spaceBounds.c1[1])));
    int xmax = std::min(shape.w, int(std::ceil(spaceBounds.c2[0])));
    int ymax = std::min(shape.h, int(std::ceil(spaceBounds.c2[1])));
    for(int i = ymin; i < ymax; i++) {
        for(int j = xmin; j < xmax; j++) {
            if(index(j, i)) {
                index(j, i)->draw();
            } else {
                std::cerr << "Warning: tried to draw an uninitialized tile at ("
                          << j << ", " << i << ")\n";
            }
        }
    }
}