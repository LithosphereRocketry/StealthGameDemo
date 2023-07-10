#include <SDL2/SDL.h>
#include "WorldSprite.h"

SDL_Point Camera::toScreenSpace(Vector<float, 2> pos) const {
    int w, h;
    SDL_GetRendererOutputSize(renderer->target, &w, &h);
    Vector<float, 2> scrctr = {w/2.0f, h/2.0f};
    Vector<float, 2> posdraw = pos - position;
    Vector<float, 2> posscr = Vector<float, 2>({ posdraw[0]*pxUnitX,
                                                -posdraw[1]*pxUnitY }) + scrctr;

    return {int(posscr[0]), int(posscr[1])};
}

SDL_Rect Camera::toScreenSpace(BoundingBox<float> box) const {
    SDL_Point tl = toScreenSpace(box.c1);
    SDL_Point br = toScreenSpace(box.c2);
    return {
        tl.x, tl.y,
        br.x-tl.x, br.y-tl.y
    };
}

void WorldSprite::load(Camera* camera) {
    cam = camera;
    Sprite::load(cam->renderer);
}


int WorldSprite::render(Vector<float, 2> loc, uint8_t alpha) {
    SDL_Rect screenloc = cam->toScreenSpace(bbox + loc);
    return Sprite::render(&screenloc, alpha);
}