#ifndef WORLDSPRITE_H
#define WORLDSPRITE_H

#include <string>
#include <SDL2/SDL.h>

#include "Sprite.h"
#include "VectorMath.h"
#include "BoundingBox.h"

class Camera {
    public:
        CachedRenderer* renderer;
        Vector<float, 2> position;
        float pxUnitX;
        float pxUnitY;
        SDL_Point toScreenSpace(Vector<float, 2> pos) const;
        SDL_Rect toScreenSpace(BoundingBox<float> box) const;
        Vector<float, 2> toWorldSpace(SDL_Point pos) const;
        BoundingBox<float> toWorldSpace(SDL_Rect box) const;
        BoundingBox<float> visibleBounds();
};

class WorldSprite: private Sprite {
    public:
        WorldSprite(const std::string& path, SDL_Rect clipMask,
                    BoundingBox<float> box):
                Sprite(path, clipMask), bbox(box), cam(nullptr) {}
        void load(Camera* camera);
        int render(Vector<float, 2> loc, uint8_t alpha = SDL_ALPHA_OPAQUE);
        BoundingBox<float> bbox;
        inline Camera* camera() { return cam; }
    private:
        Camera* cam;
};

#endif