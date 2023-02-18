#ifndef SPRITE_H
#define SPRITE_H

#include <string>

#include <SDL2/SDL.h>

#include "Caching.h"

// Wrapper class for any image that can be drawn to the screen
// This is not memoized and exists per-instance
// I might change this eventually, but not sure
class Sprite {
    public:
        Sprite() {}
        Sprite(const std::string path, SDL_Rect* clipMask) {
            surf = CachedRenderer::fetchSurface(path);
            if(clipMask) {
                txmask = *clipMask;
            } else {
                txmask = {0, 0, surf->w, surf->h};
            }
        }
        inline void load(CachedRenderer* renderer) {
            rend = renderer;
            tx = rend->fetchTexture(surf);
        }
        inline int render(uint8_t alpha = SDL_ALPHA_OPAQUE) { return render(NULL, alpha); }
        inline int render(int x, int y, uint8_t alpha = SDL_ALPHA_OPAQUE) {
            SDL_Rect tgtbox = {x, y, -1, -1};
            tgtbox.w = txmask.w;
            tgtbox.h = txmask.h;
            SDL_QueryTexture(tx, NULL, NULL, &tgtbox.w, &tgtbox.h);
            return render(&tgtbox, alpha);
        }
        inline int render(SDL_Rect* location, uint8_t alpha = SDL_ALPHA_OPAQUE) {
            SDL_SetTextureAlphaMod(tx, alpha);
            if(tx) {
                if(rend && rend->target) {
                    int err = SDL_RenderCopy(rend->target, tx, &txmask, location);
                    if(err < 0) {
                        std::cerr << "Warning: rendering failed with error " << SDL_GetError() << "\n";
                    }
                    return err;
                } else {
                    std::cerr << "Warning: tried to render to a nonexistent renderer\n";
                    return 0;
                }
            } else {
                std::cerr << "Warning: tried to render a nonexistent texture\n";
                return 0;
            }
        }
    private:
        CachedRenderer* rend = NULL;
        SDL_Surface* surf = NULL;
        SDL_Texture* tx = NULL;
        SDL_Rect txmask;
};

#endif