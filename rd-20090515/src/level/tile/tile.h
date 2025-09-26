// level/tile/tile.h — tile registry, per-face textures, render helpers

#ifndef TILE_H
#define TILE_H

#include "../level.h"
#include "..//tessellator.h"

typedef struct Tile Tile;

struct Tile {
    int id;
    int textureId;
    int  (*getTexture)(const Tile* self, int face);
    void (*render)(const Tile* self, Tessellator* t, const Level* lvl, int layer, int x, int y, int z);
    void (*onTick)(const Tile* self, Level* lvl, int x, int y, int z);
};

// Global registry, index by tile id (0..255)
extern const Tile* gTiles[256];

// Predefined tiles
extern Tile TILE_ROCK;      // id=1
extern Tile TILE_GRASS;     // id=2 (custom per face)
extern Tile TILE_DIRT;      // id=3
extern Tile TILE_STONEBRICK;// id=4
extern Tile TILE_WOOD;      // id=5

void Tile_registerAll(void);

void Face_render(Tessellator* t, int x, int y, int z, int face);

#endif