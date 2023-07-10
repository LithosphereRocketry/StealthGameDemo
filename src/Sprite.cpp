#include "Sprite.h"


Sprite::Sprite(const std::string& path, SDL_Rect* clipMask) {
    surf = CachedRenderer::fetchSurface(path);
    if(clipMask) {
        txmask = *clipMask;
    } else {
        txmask = {0, 0, surf->w, surf->h};
    }
}

void Sprite::load(CachedRenderer* renderer) {
    rend = renderer;
    tx = rend->fetchTexture(surf);
}

int Sprite::render(int x, int y, uint8_t alpha) {
    SDL_Rect tgtbox = {x, y, -1, -1};
    tgtbox.w = txmask.w;
    tgtbox.h = txmask.h;
    SDL_QueryTexture(tx, nullptr, nullptr, &tgtbox.w, &tgtbox.h);
    return render(&tgtbox, alpha);
}

int Sprite::render(SDL_Rect* loc, uint8_t alpha) {
    SDL_SetTextureAlphaMod(tx, alpha);
    if(tx) {
        if(rend && rend->target) {
            int err = SDL_RenderCopy(rend->target, tx, &txmask, loc);
            if(err < 0) {
                std::cerr << "Warning: rendering failed with error "
                            << SDL_GetError() << "\n";
            }
            return err;
        } else {
            std::cerr <<
                "Warning: tried to render to a nonexistent renderer\n";
            return 0;
        }
    } else {
        std::cerr << "Warning: tried to render a nonexistent texture\n";
        return 0;
    }
}