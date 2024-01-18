#include <SDL2/SDL.h>
#include "WorldSprite.h"

SDL_Point Camera::toScreenSpace(Vec2f pos) const {
    int w, h;
    SDL_GetRendererOutputSize(renderer->target, &w, &h);
    Vec2f scrctr = {w/2.0f, h/2.0f};
    Vec2f posdraw = pos - position;
    Vec2f posscr = Vec2f({ posdraw[0]*pxUnitX, -posdraw[1]*pxUnitY }) + scrctr;
    
    return {int(posscr[0]), int(posscr[1])};
}

SDL_Rect Camera::toScreenSpace(BoundingBox<float> box) const {
    SDL_Point tl = toScreenSpace(box.c1);
    SDL_Point br = toScreenSpace(box.c2);
    SDL_Rect newbox = { tl.x, tl.y, br.x-tl.x, br.y-tl.y };
    if(newbox.w < 0) {
        newbox.w = -newbox.w;
        newbox.x -= newbox.w;
    }
    if(newbox.h < 0) {
        newbox.h = -newbox.h;
        newbox.y -= newbox.h;
    }
    return newbox;
}

Vec2f Camera::toWorldSpace(SDL_Point pos) const {
    int w, h;
    SDL_GetRendererOutputSize(renderer->target, &w, &h);
    Vec2f posdraw = {float(pos.x - w/2.0f)/float(pxUnitX),
                                float(pos.y - h/2.0f)/float(pxUnitY)};
    Vec2f worldPos = posdraw + position;
    return worldPos;
}

BoundingBox<float> Camera::toWorldSpace(SDL_Rect box) const {
    BoundingBox<float> newbox = { toWorldSpace(SDL_Point({box.x, box.y})),
                        toWorldSpace(SDL_Point({box.x+box.w, box.y+box.h})) };
    if(newbox.c1[0] > newbox.c2[0]) { std::swap(newbox.c1[0], newbox.c2[0]); };
    if(newbox.c1[1] > newbox.c2[1]) { std::swap(newbox.c1[1], newbox.c2[1]); };
    return newbox;
}

BoundingBox<float> Camera::visibleBounds() {
    int w, h;
    SDL_GetRendererOutputSize(renderer->target, &w, &h);
    return toWorldSpace({0, 0, w, h});
}

void WorldSprite::load(Camera* newCam) {
    if(newCam) {
        if(cam != newCam || cam->renderer != newCam->renderer) {
            Sprite::load(newCam->renderer);
        }
        cam = newCam;
    }    
}


int WorldSprite::render(Vec2f loc, uint8_t alpha) {
    SDL_Rect screenloc = cam->toScreenSpace(bbox + loc);
    return Sprite::render(&screenloc, alpha);
}