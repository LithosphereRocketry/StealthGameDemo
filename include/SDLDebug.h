#include <SDL2/SDL.h>
#include "VectorMath.h"

/**
 * Draws a vector based on the given viewport information, with pretty arrow
*/
SDL_Point toScreenSpace(SDL_Renderer* rend, Vector<float, 2> pos, float px_unit) {
    int w, h;
    SDL_GetRendererOutputSize(rend, &w, &h);
    Vector<float, 2> scrctr = {w/2, h/2};
    Vector<float, 2> posflip = {pos[0], -pos[1]};
    Vector<float, 2> posscr = posflip * px_unit + scrctr;

    return {int(posscr[0]), int(posscr[1])};
}

void drawVector(SDL_Renderer* rend, Vector<float, 2> vec, Vector<float, 2> origin, Vector<float, 2> campos, float px_unit) {
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