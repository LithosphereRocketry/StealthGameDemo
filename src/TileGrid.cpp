#include "TileGrid.h"
#include "utilities.h"

TileGrid::TileGrid(int w, int h, float tw, float th):
        shape({0, 0, w, h}), tileWidth(tw), tileHeight(th), buffer(w*h),
        drawBound({0, 0}, {0, 0}), collisionBound({0, 0}, {0, 0}),
        activeCam(nullptr) {
}

void TileGrid::put(TilePrototype* prototype, int x, int y) {
    Vec2f p = {x*tileWidth, y*tileHeight};
    drawBound |= prototype->getBounds();
    collisionBound |= prototype->getBounds(); 
    // TODO: this should probably be handled separately
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
    int xmin = std::max(0, saturating_cast<int>(std::floor(spaceBounds.c1[0])));
    int ymin = std::max(0, saturating_cast<int>(std::floor(spaceBounds.c1[1])));
    int xmax = std::min(shape.w, saturating_cast<int>(std::ceil(spaceBounds.c2[0])));
    int ymax = std::min(shape.h, saturating_cast<int>(std::ceil(spaceBounds.c2[1])));
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

FreePathResult TileGrid::getFreePath(const Ray2f& path, float radius, float bound) {
    BoundingBox<float> startBound = {
        path.origin - collisionBound.c2 - Vec2f({radius, radius}),
        path.origin - collisionBound.c1 + Vec2f({radius, radius})
    };
    BoundingBox<float> endBound = {
        path.origin - collisionBound.c2 + path.dir*bound - Vec2f({radius, radius}),
        path.origin - collisionBound.c1 + path.dir*bound + Vec2f({radius, radius})
    };
    BoundingBox<float> moveBound = startBound | endBound;

    int xmin = std::max(0, saturating_cast<int>(std::floor(moveBound.c1[0])));
    int ymin = std::max(0, saturating_cast<int>(std::floor(moveBound.c1[1])));
    int xmax = std::min(shape.w, saturating_cast<int>(std::ceil(moveBound.c2[0])));
    int ymax = std::min(shape.h, saturating_cast<int>(std::ceil(moveBound.c2[1])));
    // int xmin = 0;
    // int ymin = 0;
    // int xmax = shape.w;
    // int ymax = shape.h;

    FreePathResult result = {FREE, INFINITY, INFINITY, nullptr};
    for(int i = ymin; i < ymax; i++) {
        for(int j = xmin; j < xmax; j++) {
            if(!index(j, i)) { continue; }
            FreePathResult newres = index(j, i)->getFreePath(path, radius, bound);
            if(newres.dist < result.dist) {
                result = newres;
            }
        }
    }
    return result;
}