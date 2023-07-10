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
        Sprite(const std::string& path, SDL_Rect* clipMask);

        void load(CachedRenderer* renderer);
        inline int render(uint8_t alpha = SDL_ALPHA_OPAQUE) {
            return render(nullptr, alpha);
        }
        int render(int x, int y, uint8_t alpha = SDL_ALPHA_OPAQUE);
        int render(SDL_Rect* loc, uint8_t alpha = SDL_ALPHA_OPAQUE);
    private:
        CachedRenderer* rend = nullptr;
        SDL_Surface* surf = nullptr;
        SDL_Texture* tx = nullptr;
        SDL_Rect txmask;
};

#endif