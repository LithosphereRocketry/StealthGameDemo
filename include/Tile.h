#ifndef TILE_H
#define TILE_H

#include <string>
#include <math.h>

#include <SDL2/SDL.h>

#include "Caching.h"
#include "Sprite.h"

// Class for things that have a "real-life" size and position
// This is where camera/zoom scaling and stuff happens
class Tile {
    public:
        SDL_Point position;
        SDL_Rect graphicsBox;

        Tile() {}
        Tile(std::string texturepath, SDL_Rect* location): sprite(texturepath, location) {}

        void load(CachedRenderer* dest) {
            sprite.load(dest);
        }
        int draw(float camx, float camy, float zoom) {
            SDL_Rect screenpos = {
                (int) round((position.x+graphicsBox.x-camx)*zoom),
                (int) round((position.y+graphicsBox.y-camy)*zoom),
                (int) round(graphicsBox.w*zoom),
                (int) round(graphicsBox.h*zoom)
            };
            return sprite.render(&screenpos);
        }
    private:
        Sprite sprite;
};

// Class for tile grids
// This class "owns" all of its tiles
class TileGrid {
    public:
        TileGrid(size_t w, size_t h, int tw, int th) {
            width = w;
            height = h;
            tileWidth = tw;
            tileHeight = th;
            buffer = new Tile*[width*height](); // () needed here to make sure the tiles get initialized as nullptr
        }
        ~TileGrid() {
            delete[] buffer;
        }
        Tile** operator [] (size_t i) {
            return buffer + width*i;
        }
        void fill(const Tile* prototype) {
            fillRect(prototype, 0, 0, width, height);
        }
        void fillRect(const Tile* prototype, int x, int y, int w, int h) {
            for(int i = y; i < y+h; i++) {
                for(int j = x; j < x+w; j++) {
                    if(buffer[width*i + j]) {
                        delete buffer[width*i + j];
                    }
                    buffer[width*i + j] = new Tile(*prototype);
                    buffer[width*i + j]->position = {i*tileWidth, j*tileHeight};
                }
            }
        }
        void load(CachedRenderer* dest) {
            for(int i = 0; i < width*height; i++) {
                if(!buffer[i]) {
                    std::cerr << "Warning: tried to load an uninitialized tile\n";
                }
                buffer[i]->load(dest);
            }
        }
        void draw(float camx, float camy, float zoom) {
            for(int i = 0; i < width*height; i++) {
                if(!buffer[i]) {
                    std::cerr << "Warning: tried to draw an uninitialized tile\n";
                }
                buffer[i]->draw(camx, camy, zoom);
            }
        }
    private:
        size_t width;
        size_t height;
        int tileWidth;
        int tileHeight;
        Tile** buffer;
};

#endif