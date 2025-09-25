// level/chunk.c — chunk display lists, rebuild & render

#include "chunk.h"
#include "../tile/tile.h"
#include "tessellator.h"
#include "../textures.h"

extern Tessellator TESSELLATOR;

// global stats (keep parity with original Java static fields)
int CHUNK_RebuiltThisFrame = 0;
int CHUNK_Updates          = 0;

void Chunk_init(Chunk* c, Level* level, int minX, int minY, int minZ, int maxX, int maxY, int maxZ) {
    c->level = level;
    c->texture = loadTexture("resources/terrain.png", GL_NEAREST);

    c->minX = minX; c->minY = minY; c->minZ = minZ;
    c->maxX = maxX; c->maxY = maxY; c->maxZ = maxZ;
    c->dirty = true;

    c->boundingBox = AABB_create(minX, minY, minZ, maxX, maxY, maxZ);
    c->lists = glGenLists(2);
}

void Chunk_rebuild(Chunk* c, int layer) {
    if (CHUNK_RebuiltThisFrame == 2) return;

    CHUNK_Updates++;
    CHUNK_RebuiltThisFrame++;

    c->dirty = false;

    glNewList(c->lists + layer, GL_COMPILE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, c->texture);
    Tessellator_init(&TESSELLATOR);

    for (int x = c->minX; x < c->maxX; ++x) {
        for (int y = c->minY; y < c->maxY; ++y) {
            for (int z = c->minZ; z < c->maxZ; ++z) {
                int tileId = Level_getTile(c->level, x, y, z);
                if (tileId > 0) {
                    const Tile* t = gTiles[tileId];
                    if (t && t->render) {
                        t->render(t, &TESSELLATOR, c->level, layer, x, y, z);
                    }
                }
            }
        }
    }

    Tessellator_flush(&TESSELLATOR);
    glDisable(GL_TEXTURE_2D);
    glEndList();
}

void Chunk_render(Chunk* c, int layer) {
    if (c->dirty) {
        Chunk_rebuild(c, 0);
        Chunk_rebuild(c, 1);
    }
    glCallList(c->lists + layer);
}

void Chunk_setDirty(Chunk* c) { c->dirty = true; }