#ifndef CACHED_RENDERER_H
#define CACHED_RENDERER_H

#include <unordered_map>
#include <string>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>

class CachedRenderer {
    public:
        SDL_Renderer* target;
        CachedRenderer(SDL_Renderer* renderer) {
            target = renderer;
        }
        inline SDL_Texture* fetchTexture(const std::string path) {
            printf("Loading %s\n", path.c_str());
            if(!cache[path]) {
                printf("Cache miss\n");
                cache[path] = IMG_LoadTexture(target, path.c_str());
            }
            return cache[path];
        }
        inline void display() { SDL_RenderPresent(target); }
    private:
        std::unordered_map<std::string, SDL_Texture*> cache;
};

#endif