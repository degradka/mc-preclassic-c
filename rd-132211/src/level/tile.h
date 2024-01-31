#ifndef TILE_H
#define TILE_H

#include "level.h"
#include "tessellator.h"

typedef struct {
    // Any specific properties of Tile can be added here
} Tile;

extern Tile rock;

void Tile_render(Tile* tile, Tessellator* tessellator, const Level* level, int x, int y, int z);

#endif  // TILE_H
