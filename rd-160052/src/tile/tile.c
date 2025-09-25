// tile/tile.c — registry, per-face textures, render helpers

#include "tile.h"
#include <string.h>

static int Tile_default_getTexture(const Tile* self, int face) {
    (void)face;
    return self->textureId;
}

static inline int shouldRenderFace(const Level* lvl, int x, int y, int z, int layer) {
    // match Java logic: face visible if not solid AND (isLit XOR (layer==1))
    const int lit = (x < 0 || y < 0 || z < 0 || x >= lvl->width || y >= lvl->depth || z >= lvl->height)
                  ? 1
                  : (y >= lvl->lightDepths[x + z * lvl->width]);
    return (!Level_isSolidTile(lvl, x, y, z)) && ((lit ^ (layer == 1)) != 0);
}

// helper to compute UVs from atlas slot (16x16 tiles on 256x256)
static void calcUV(int slot, float* u0, float* v0, float* u1, float* v1) {
    // 16x16 tiles in a 256x256 atlas
    float minU = (slot % 16) / 16.0f;
    float minV = (slot / 16) / 16.0f;
    *u0 = minU;
    *v0 = minV;
    *u1 = minU + 16.0f / 256.0f;
    *v1 = minV + 16.0f / 256.0f;
}

static void Tile_render_shared(const Tile* self, Tessellator* t, const Level* lvl, int layer, int x, int y, int z) {
    const float shadeX = 0.6f, shadeY = 1.0f, shadeZ = 0.8f;

    const float minX = (float)x, maxX = (float)x + 1.0f;
    const float minY = (float)y, maxY = (float)y + 1.0f;
    const float minZ = (float)z, maxZ = (float)z + 1.0f;

    float u0, v0, u1, v1;

    // bottom (face 0)
    if (shouldRenderFace(lvl, x, y - 1, z, layer)) {
        Tessellator_color(t, 1.0f, 1.0f, 1.0f);
        calcUV(self->getTexture(self, 0), &u0,&v0,&u1,&v1);
        Tessellator_vertexUV(t, minX, minY, maxZ, u0, v1);
        Tessellator_vertexUV(t, minX, minY, minZ, u0, v0);
        Tessellator_vertexUV(t, maxX, minY, minZ, u1, v0);
        Tessellator_vertexUV(t, maxX, minY, maxZ, u1, v1);
    }

    // top (face 1)
    if (shouldRenderFace(lvl, x, y + 1, z, layer)) {
        Tessellator_color(t, shadeY, shadeY, shadeY);
        calcUV(self->getTexture(self, 1), &u0, &v0, &u1, &v1);
        Tessellator_vertexUV(t, maxX, maxY, maxZ, u1, v1);
        Tessellator_vertexUV(t, maxX, maxY, minZ, u1, v0);
        Tessellator_vertexUV(t, minX, maxY, minZ, u0, v0);
        Tessellator_vertexUV(t, minX, maxY, maxZ, u0, v1);
    }

    // -Z (face 2)
    if (shouldRenderFace(lvl, x, y, z - 1, layer)) {
        Tessellator_color(t, shadeZ, shadeZ, shadeZ);
        calcUV(self->getTexture(self, 2), &u0, &v0, &u1, &v1);
        Tessellator_vertexUV(t, minX, maxY, minZ, u1, v0);
        Tessellator_vertexUV(t, maxX, maxY, minZ, u0, v0);
        Tessellator_vertexUV(t, maxX, minY, minZ, u0, v1);
        Tessellator_vertexUV(t, minX, minY, minZ, u1, v1);
    }

    // +Z (face 3)
    if (shouldRenderFace(lvl, x, y, z + 1, layer)) {
        Tessellator_color(t, shadeZ, shadeZ, shadeZ);
        calcUV(self->getTexture(self, 3), &u0, &v0, &u1, &v1);
        Tessellator_vertexUV(t, minX, maxY, maxZ, u0, v0);
        Tessellator_vertexUV(t, minX, minY, maxZ, u0, v1);
        Tessellator_vertexUV(t, maxX, minY, maxZ, u1, v1);
        Tessellator_vertexUV(t, maxX, maxY, maxZ, u1, v0);
    }

    // -X (face 4)
    if (shouldRenderFace(lvl, x - 1, y, z, layer)) {
        Tessellator_color(t, shadeX, shadeX, shadeX);
        calcUV(self->getTexture(self, 4), &u0, &v0, &u1, &v1);
        Tessellator_vertexUV(t, minX, maxY, maxZ, u1, v0);
        Tessellator_vertexUV(t, minX, maxY, minZ, u0, v0);
        Tessellator_vertexUV(t, minX, minY, minZ, u0, v1);
        Tessellator_vertexUV(t, minX, minY, maxZ, u1, v1);
    }

    // +X (face 5)
    if (shouldRenderFace(lvl, x + 1, y, z, layer)) {
        Tessellator_color(t, shadeX, shadeX, shadeX);
        calcUV(self->getTexture(self, 5), &u0, &v0, &u1, &v1);
        Tessellator_vertexUV(t, maxX, minY, maxZ, u0, v1);
        Tessellator_vertexUV(t, maxX, minY, minZ, u1, v1);
        Tessellator_vertexUV(t, maxX, maxY, minZ, u1, v0);
        Tessellator_vertexUV(t, maxX, maxY, maxZ, u0, v0);
    }
}

/* ---------- tile instances & registry ---------- */

const Tile* gTiles[256] = { 0 };

static void registerTile(Tile* t, int id, int tex, int (*getTex)(const Tile*,int)) {
    t->id = id; t->textureId = tex;
    t->getTexture = getTex ? getTex : Tile_default_getTexture;
    t->render = Tile_render_shared;
    t->onTick = NULL;
    gTiles[id] = t;
}

/* Rock (single texture) */
Tile TILE_ROCK;
/* Dirt (single texture) */
Tile TILE_DIRT;
/* Stone brick (single texture) */
Tile TILE_STONEBRICK;
/* Wood (single texture) */
Tile TILE_WOOD;

/* Grass (per-face textures) — face map: top=0, bottom=2, sides=3 */
static int Grass_getTexture(const Tile* self, int face) {
    (void)self;
    return (face == 1) ? 0 : (face == 0) ? 2 : 3;
}

Tile TILE_GRASS;

static void Grass_onTick(const Tile* self, Level* lvl, int x, int y, int z) {
    (void)self;
    if (Level_isLit(lvl, x, y, z)) {
        // try 4 random neighbors
        for (int i = 0; i < 4; ++i) {
            int tx = x + (rand() % 3) - 1;  // [-1..+1]
            int ty = y + (rand() % 5) - 3;  // [-3..+1] (matches Java’s skew)
            int tz = z + (rand() % 3) - 1;  // [-1..+1]
            if (Level_getTile(lvl, tx, ty, tz) == TILE_DIRT.id && Level_isLit(lvl, tx, ty, tz)) {
                level_setTile(lvl, tx, ty, tz, TILE_GRASS.id);
            }
        }
    } else {
        // no sunlight: turn into dirt
        level_setTile(lvl, x, y, z, TILE_DIRT.id);
    }
}


void Tile_registerAll(void) {
    memset((void*)gTiles, 0, sizeof(gTiles));
    registerTile(&TILE_ROCK,       1,  1, NULL);
    registerTile(&TILE_GRASS,      2,  3, Grass_getTexture);
    registerTile(&TILE_DIRT,       3,  2, NULL);
    registerTile(&TILE_STONEBRICK, 4, 16, NULL);
    registerTile(&TILE_WOOD,       5,  4, NULL);

    TILE_GRASS.onTick = Grass_onTick;
}

/* ---------- untextured single-face helper (for hit highlight) ---------- */
void Face_render(Tessellator* t, int x, int y, int z, int face) {
    const float minX = (float)x,     maxX = (float)x + 1.0f;
    const float minY = (float)y,     maxY = (float)y + 1.0f;
    const float minZ = (float)z,     maxZ = (float)z + 1.0f;

    if (face == 0) { // bottom
        Tessellator_vertex(t, minX, minY, maxZ);
        Tessellator_vertex(t, minX, minY, minZ);
        Tessellator_vertex(t, maxX, minY, minZ);
        Tessellator_vertex(t, maxX, minY, maxZ);
    } else if (face == 1) { // top
        Tessellator_vertex(t, maxX, maxY, maxZ);
        Tessellator_vertex(t, maxX, maxY, minZ);
        Tessellator_vertex(t, minX, maxY, minZ);
        Tessellator_vertex(t, minX, maxY, maxZ);
    } else if (face == 2) { // -Z
        Tessellator_vertex(t, minX, maxY, minZ);
        Tessellator_vertex(t, maxX, maxY, minZ);
        Tessellator_vertex(t, maxX, minY, minZ);
        Tessellator_vertex(t, minX, minY, minZ);
    } else if (face == 3) { // +Z
        Tessellator_vertex(t, minX, maxY, maxZ);
        Tessellator_vertex(t, minX, minY, maxZ);
        Tessellator_vertex(t, maxX, minY, maxZ);
        Tessellator_vertex(t, maxX, maxY, maxZ);
    } else if (face == 4) { // -X
        Tessellator_vertex(t, minX, maxY, maxZ);
        Tessellator_vertex(t, minX, maxY, minZ);
        Tessellator_vertex(t, minX, minY, minZ);
        Tessellator_vertex(t, minX, minY, maxZ);
    } else if (face == 5) { // +X
        Tessellator_vertex(t, maxX, minY, maxZ);
        Tessellator_vertex(t, maxX, minY, minZ);
        Tessellator_vertex(t, maxX, maxY, minZ);
        Tessellator_vertex(t, maxX, maxY, maxZ);
    }
}
