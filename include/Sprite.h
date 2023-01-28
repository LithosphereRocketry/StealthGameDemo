#ifndef SPRITE_H
#define SPRITE_H

#include <string>

#include <SDL2/SDL.h>

#include "CachedRenderer.h"

// Wrapper class for simple sprites
// Sprites must be bound to a specific renderer
class Sprite {
    public:
        /*
        Sprite(CachedRenderer* renderer, char* path, <optional> SDL_Rect* mask = NULL)
        Creates a sprite on the given renderer with the given image path and mask
        renderer: SDL_Renderer to target; must continue to exist for the lifetime of the sprite
        path: C string path to image, used only in constructor
        mask: Portion of image to draw; used only in constructor
        */
        Sprite(CachedRenderer* renderer, const std::string path): Sprite(renderer, path, NULL) {}
        Sprite(CachedRenderer* renderer, const std::string path, SDL_Rect* mask) {
            rend = renderer;
            if(mask) {
                txmask = new SDL_Rect;
                *txmask = *mask;
            }
            tx = rend->fetchTexture(path);
        }
        ~Sprite() {
            delete txmask;
        }
        inline int render() { return render(NULL); }
        inline int render(int x, int y) {
            SDL_Rect tgtbox = {x, y, -1, -1};
            if(txmask) {
                tgtbox.w = txmask->w;
                tgtbox.h = txmask->h;
            } else {            
                SDL_QueryTexture(tx, NULL, NULL, &tgtbox.w, &tgtbox.h);
            }
            return render(&tgtbox);
        }
        inline int render(SDL_Rect* location) {
            return SDL_RenderCopy(rend->target, tx, txmask, location);
        }
    private:
        CachedRenderer* rend = NULL;
        SDL_Texture* tx = NULL;
        SDL_Rect* txmask = NULL;

};

#endif