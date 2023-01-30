#include "Caching.h"

std::unordered_map<std::string, SDL_Surface*> CachedRenderer::surfaceCache = {};