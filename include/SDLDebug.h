#ifndef SDL_DEBUG_H
#define SDL_DEBUG_H

#include <vector>
#include <iostream>
#include "math.h"
#include <SDL2/SDL.h>

#include "VectorMath.h"
#include "BoundingBox.h"
#include "WorldSprite.h"

#define SDL_DEBUG_CIRCLE_PTS 20

inline void drawInfLine(Camera* rend, Vec2f origin,
            Vec2f normal) {
    SDL_Point originpt = rend->toScreenSpace(origin);
    SDL_Point offspt = rend->toScreenSpace(origin+normal.orthogonal());
    SDL_Point scroffs = {offspt.x-originpt.x, offspt.y-originpt.y};
    int w, h;
    SDL_GetRendererOutputSize(rend->renderer->target, &w, &h);
    if(scroffs.y == 0) {
        SDL_Point points[] = {{0, originpt.y}, {w, originpt.y}};
        SDL_RenderDrawLines(rend->renderer->target, points, 2);
    } else {
        SDL_Point toppt = {originpt.x - originpt.y*scroffs.x/scroffs.y, 0};
        SDL_Point botpt = {originpt.x - (originpt.y-h)*scroffs.x/scroffs.y, h};
        SDL_Point points[] = {toppt, botpt};
        SDL_RenderDrawLines(rend->renderer->target, points, 2);
    }
}

inline void drawVector(Camera* rend, Vec2f vec, Vec2f origin) {
    Vec2f corner1 = origin + vec*0.9 + vec.orthogonal()*0.1; 
    Vec2f corner2 = origin + vec*0.9 - vec.orthogonal()*0.1;
    SDL_Point points[] = {
        rend->toScreenSpace(origin),
        rend->toScreenSpace(origin+vec),
        rend->toScreenSpace(corner1),
        rend->toScreenSpace(corner2),
        rend->toScreenSpace(origin+vec)
    };
    SDL_RenderDrawLines(rend->renderer->target, points, 5);
}

inline void drawPoint(Camera* rend, Vec2f pos, float radius) {
    static const double pi = std::atan(1.0)*4; // TODO: migrate to C++20 pi?
    SDL_Point points[SDL_DEBUG_CIRCLE_PTS + 1];
    for(size_t i = 0; i < SDL_DEBUG_CIRCLE_PTS + 1; i++) {
        float angle = i*2*pi / SDL_DEBUG_CIRCLE_PTS;
        points[i] = rend->toScreenSpace({pos[0] + std::sin(angle)*radius,
                                        pos[1] + std::cos(angle)*radius});
    }
    SDL_RenderDrawLines(rend->renderer->target, points,
        SDL_DEBUG_CIRCLE_PTS + 1);
}

inline void drawPoly(Camera* rend, std::vector<Vec2f> verts) {
    SDL_Point* points = new SDL_Point[verts.size()+1];
    for(size_t i = 0; i < verts.size(); i++) {
        points[i] = rend->toScreenSpace(verts[i]);
    }
    points[verts.size()] = points[0];
    SDL_RenderDrawLines(rend->renderer->target, points, verts.size()+1);
    delete[] points;
}

inline void drawBBox(Camera* rend, BoundingBox<float> box) {
    drawPoly(rend, std::vector<Vec2f>({
        box.tl(),
        box.tr(),
        box.br(),
        box.bl()
    }));
}

inline std::ostream& operator << (std::ostream& os, SDL_Point p) {
    os << "<" << p.x << ", " << p.y << ">";
    return os;
}

inline std::ostream& operator << (std::ostream& os, SDL_Rect r) {
    os << "<" << r.x << ", " << r.y << ", " << r.w << ", " << r.h << ">";
    return os;
}

#endif