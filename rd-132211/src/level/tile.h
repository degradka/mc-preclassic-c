#ifndef TILE_H
#define TILE_H

#include "level.h"
#include "tessellator.h"

typedef struct {
    int textureId;
} Tile;

extern Tile grass;
extern Tile rock;

void Tile_render(Tile* tile, Tessellator* tessellator, const Level* level, int layer, int x, int y, int z);
void Face_render(Tessellator* tessellator, int x, int y, int z, int face);

#endif  // TILE_H
