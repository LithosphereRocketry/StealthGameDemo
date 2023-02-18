#ifndef CACHING_H
#define CACHING_H

#include <unordered_map>
#include <string>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>


class CachedRenderer {
    public:
        SDL_Renderer* target;
        SDL_Point center;
        CachedRenderer(SDL_Renderer* renderer) {
            target = renderer;
            int w, h;
            SDL_GetRendererOutputSize(renderer, &w, &h);
            center = {w/2, h/2};
        }
        ~CachedRenderer() {
            for(auto p : textureCache) {
                SDL_DestroyTexture(p.second);
            }
        }
        // Rule of five safety things
        CachedRenderer(const CachedRenderer& other) {
            std::cerr << "CachedRenderer copy constructor is not safe\n";
        }
        CachedRenderer(CachedRenderer&& other) {
            std::cerr << "CachedRenderer move constructor is not safe\n";
        }
        inline CachedRenderer& operator=(const CachedRenderer& other) {
            std::cerr << "CachedRenderer copy assignment is not safe\n";
            return *(new CachedRenderer(nullptr)); // this is an awful line and a memory leak but who cares this is an error case
        }
        inline CachedRenderer& operator=(CachedRenderer&& other) {
            std::cerr << "CachedRenderer move assignment is not safe\n";
            return *(new CachedRenderer(nullptr));
        }
        static inline SDL_Surface* fetchSurface(const std::string path) {
            if(!surfaceCache[path]) {
                surfaceCache[path] = IMG_Load(path.c_str());
            }
            return surfaceCache[path];
        }
        inline SDL_Texture* fetchTexture(SDL_Surface* surface) {
            if(!textureCache[surface]) {
                textureCache[surface] = SDL_CreateTextureFromSurface(target, surface);
            }
            return textureCache[surface];
        }
        inline void display() { SDL_RenderPresent(target); }
    private:
        static constexpr bool weirdfalse = false;
        static std::unordered_map<std::string, SDL_Surface*> surfaceCache;
        std::unordered_map<SDL_Surface*, SDL_Texture*> textureCache;
};

#endif