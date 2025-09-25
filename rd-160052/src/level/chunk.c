// level/chunk.c — chunk display lists, rebuild & render

#include "chunk.h"
#include "../tile/tile.h"
#include "tessellator.h"
#include "../textures.h"
#include "../timer.h"
#include "../player.h"
#include <math.h>

extern Tessellator TESSELLATOR;

// (optional stats)
static long long CHUNK_TotalTimeNS  = 0;
static int       CHUNK_TotalUpdates = 0;

void Chunk_init(Chunk* c, Level* level, int minX, int minY, int minZ, int maxX, int maxY, int maxZ) {
    c->level   = level;
    c->texture = loadTexture("resources/terrain.png", GL_NEAREST);

    c->minX = minX; c->minY = minY; c->minZ = minZ;
    c->maxX = maxX; c->maxY = maxY; c->maxZ = maxZ;
    c->dirty = true;
    c->dirtiedTime = currentTimeMillis();

    // precompute chunk center for distance checks
    c->x = (minX + maxX) * 0.5;
    c->y = (minY + maxY) * 0.5;
    c->z = (minZ + maxZ) * 0.5;

    c->boundingBox = AABB_create(minX, minY, minZ, maxX, maxY, maxZ);
    c->lists = glGenLists(2);
}

void Chunk_rebuild(Chunk* c, int layer) {
    c->dirty = false;

    glNewList(c->lists + layer, GL_COMPILE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, c->texture);
    Tessellator_init(&TESSELLATOR);

    int tiles = 0;
    // (coarse timing)
    long long nsStart = getCurrentTimeInNanoseconds();

    for (int x = c->minX; x < c->maxX; ++x)
    for (int y = c->minY; y < c->maxY; ++y)
    for (int z = c->minZ; z < c->maxZ; ++z) {
        int tileId = Level_getTile(c->level, x, y, z);
        if (tileId > 0) {
            const Tile* t = gTiles[tileId];
            if (t && t->render) {
                t->render(t, &TESSELLATOR, c->level, layer, x, y, z);
                tiles++;
            }
        }
    }

    Tessellator_flush(&TESSELLATOR);
    glDisable(GL_TEXTURE_2D);
    glEndList();

    if (tiles > 0) {
        CHUNK_TotalTimeNS += (getCurrentTimeInNanoseconds() - nsStart);
        CHUNK_TotalUpdates++;
    }
}

void Chunk_render(Chunk* c, int layer) {
    glCallList(c->lists + layer);
}

void Chunk_setDirty(Chunk* c) {
    if (!c->dirty) c->dirtiedTime = currentTimeMillis();
    c->dirty = true;
}

double Chunk_distanceToSqr(const Chunk* c, const Player* p) {
    double dx = p->e.x - c->x;
    double dy = p->e.y - c->y;
    double dz = p->e.z - c->z;
    return dx*dx + dy*dy + dz*dz;
}