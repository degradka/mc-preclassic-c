// level/tile.h

#ifndef TILE_H_
#define TILE_H_

#include "tessellator.h"
#include "level.h"

class Tile {
public:
    static Tile rock;

    void render(Tessellator* tessellator, Level* level, int x, int y, int z);
};

#endif // TILE_H_