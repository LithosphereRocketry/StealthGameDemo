#include <SDL2/SDL.h>
#include "math.h"
#include "VectorMath.h"

#define SDL_DEBUG_CIRCLE_PTS 20

/**
 * Draws a vector based on the given viewport information, with pretty arrow
*/
SDL_Point toScreenSpace(SDL_Renderer* rend, Vector<float, 2> pos,
                                                                float px_unit) {
    int w, h;
    SDL_GetRendererOutputSize(rend, &w, &h);
    Vector<float, 2> scrctr = {w/2.0f, h/2.0f};
    Vector<float, 2> posflip = {pos[0], -pos[1]};
    Vector<float, 2> posscr = posflip * px_unit + scrctr;

    return {int(posscr[0]), int(posscr[1])};
}

void drawInfLine(SDL_Renderer* rend, Vector<float, 2> origin,
            Vector<float, 2> normal, Vector<float, 2> campos, float px_unit) {
    SDL_Point originpt = toScreenSpace(rend, origin-campos, px_unit);
    SDL_Point offspt = toScreenSpace(rend, origin+normal.orthogonal()-campos, px_unit);
    SDL_Point scroffs = {offspt.x-originpt.x, offspt.y-originpt.y};
    int w, h;
    SDL_GetRendererOutputSize(rend, &w, &h);
    if(scroffs.y == 0) {
        SDL_Point points[] = {{0, originpt.y}, {w, originpt.y}};
        SDL_RenderDrawLines(rend, points, 2);
    } else {
        SDL_Point toppt = {originpt.x - originpt.y*scroffs.x/scroffs.y, 0};
        SDL_Point botpt = {originpt.x - (originpt.y-h)*scroffs.x/scroffs.y, h};
        SDL_Point points[] = {toppt, botpt};
        SDL_RenderDrawLines(rend, points, 2);
    }
}

void drawVector(SDL_Renderer* rend, Vector<float, 2> vec,
            Vector<float, 2> origin, Vector<float, 2> campos, float px_unit) {
    Vector<float, 2> corner1 = origin + vec*0.9 + vec.orthogonal()*0.1; 
    Vector<float, 2> corner2 = origin + vec*0.9 - vec.orthogonal()*0.1;
    SDL_Point points[] = {
        toScreenSpace(rend, origin-campos, px_unit),
        toScreenSpace(rend, origin+vec-campos, px_unit),
        toScreenSpace(rend, corner1-campos, px_unit),
        toScreenSpace(rend, corner2-campos, px_unit),
        toScreenSpace(rend, origin+vec-campos, px_unit)
    };
    SDL_RenderDrawLines(rend, points, 5);
}

void drawPoint(SDL_Renderer* rend, Vector<float, 2> pos,
                        Vector<float, 2> campos, float px_unit, float radius) {
    SDL_Point points[SDL_DEBUG_CIRCLE_PTS + 1];
    SDL_Point center = toScreenSpace(rend, pos-campos, px_unit);
    for(int i = 0; i < SDL_DEBUG_CIRCLE_PTS + 1; i++) {
        float angle = i*2*M_PI / SDL_DEBUG_CIRCLE_PTS;
        points[i] = {center.x + int(radius*px_unit*sinf(angle)),
                                int(center.y + radius*px_unit*cosf(angle))};
    }
    SDL_RenderDrawLines(rend, points, SDL_DEBUG_CIRCLE_PTS + 1);
}

void drawPoly(SDL_Renderer* rend, std::vector<Vector<float, 2>> verts,
            Vector<float, 2> campos, float px_unit) {
    SDL_Point* points = new SDL_Point[verts.size()+1];
    for(int i = 0; i < verts.size(); i++) {
        points[i] = toScreenSpace(rend, verts[i]-campos, px_unit);
    }
    points[verts.size()] = points[0];
    SDL_RenderDrawLines(rend, points, verts.size()+1);
    delete[] points;
}