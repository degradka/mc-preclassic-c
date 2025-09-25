// tile/tile.h — tile registry, per-face textures, render helpers

#ifndef TILE_H
#define TILE_H

#include "../level/level.h"
#include "../level/tessellator.h"

typedef struct Tile Tile;

struct Tile {
    int id;
    int textureId; // default atlas slot
    // return atlas slot for a given face (0: bottom, 1: top, 2:-Z, 3:+Z, 4:-X, 5:+X)
    int (*getTexture)(const Tile* self, int face);
    // shared render uses getTexture(self, face)
    void (*render)(const Tile* self, Tessellator* t, const Level* lvl, int layer, int x, int y, int z);
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

// Helper to render a plain, untextured face (for highlights)
void Face_render(Tessellator* t, int x, int y, int z, int face);

#endif