// level/levelrenderer.c — chunk grid, frustum culling, dirty-marking, and hit highlight

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>

#include "levelrenderer.h"
#include "level.h"
#include "chunk.h"
#include "frustum.h"
#include "../tile/tile.h"
#include "../timer.h"
#include "../hitresult.h"
#include "../player.h"
#include <math.h>
#include <stdio.h>

void LevelRenderer_init(LevelRenderer* r, Level* level) {
    r->chunkAmountX = level->width  / CHUNK_SIZE;
    r->chunkAmountY = level->depth  / CHUNK_SIZE;
    r->chunkAmountZ = level->height / CHUNK_SIZE;
    level->renderer = r;

    int total = r->chunkAmountX * r->chunkAmountY * r->chunkAmountZ;
    r->chunks = (Chunk*)malloc((size_t)total * sizeof(Chunk));
    if (!r->chunks) { fprintf(stderr, "Failed to allocate chunks\n"); exit(EXIT_FAILURE); }

    Tessellator_init(&r->tessellator);

    for (int x = 0; x < r->chunkAmountX; x++)
    for (int y = 0; y < r->chunkAmountY; y++)
    for (int z = 0; z < r->chunkAmountZ; z++) {
        int minChunkX = x * CHUNK_SIZE, maxChunkX = MIN(level->width,  (x + 1) * CHUNK_SIZE);
        int minChunkY = y * CHUNK_SIZE, maxChunkY = MIN(level->depth,  (y + 1) * CHUNK_SIZE);
        int minChunkZ = z * CHUNK_SIZE, maxChunkZ = MIN(level->height, (z + 1) * CHUNK_SIZE);
        Chunk_init(&r->chunks[(x + y * r->chunkAmountX) * r->chunkAmountZ + z],
                   level, minChunkX, minChunkY, minChunkZ, maxChunkX, maxChunkY, maxChunkZ);
    }
}

void LevelRenderer_render(const LevelRenderer* r, int layer) {
    frustum_calculate();

    int total = r->chunkAmountX * r->chunkAmountY * r->chunkAmountZ;
    for (int i = 0; i < total; ++i) {
        if (frustum_isVisible(&r->chunks[i].boundingBox)) {
            Chunk_render(&r->chunks[i], layer);
        }
    }
}

void LevelRenderer_destroy(LevelRenderer* r) {
    free(r->chunks);
}

/* --- dirty-chunk prioritization -------------------------------------------- */

// sort state for qsort comparator
static const Player* gSortPlayer = NULL;
static long long gSortNow = 0;

static int dirty_cmp(const void* a, const void* b) {
    const Chunk* c1 = *(const Chunk* const*)a;
    const Chunk* c2 = *(const Chunk* const*)b;

    if (c1 == c2) return 0;

    // visible chunks first
    int v1 = frustum_isVisible(&c1->boundingBox);
    int v2 = frustum_isVisible(&c2->boundingBox);
    if (v1 && !v2) return -1;
    if (v2 && !v1) return  1;

    // higher priority to larger dirty duration (mirror Java logic: they compare ints after /2000)
    int d1 = (int)((gSortNow - c1->dirtiedTime) / 2000LL);
    int d2 = (int)((gSortNow - c2->dirtiedTime) / 2000LL);
    if (d1 < d2) return -1;
    if (d1 > d2) return  1;

    // finally, closer to player first
    double dist1 = Chunk_distanceToSqr(c1, gSortPlayer);
    double dist2 = Chunk_distanceToSqr(c2, gSortPlayer);
    return (dist1 < dist2) ? -1 : 1;
}

void LevelRenderer_updateDirtyChunks(LevelRenderer* r, const Player* player) {
    frustum_calculate(); // ensure frustum up-to-date for visibility test

    // collect dirty chunk pointers
    int total = r->chunkAmountX * r->chunkAmountY * r->chunkAmountZ;
    Chunk** list = (Chunk**)malloc((size_t)total * sizeof(Chunk*));
    if (!list) return;

    int n = 0;
    for (int i = 0; i < total; ++i) {
        if (Chunk_isDirty(&r->chunks[i])) list[n++] = &r->chunks[i];
    }
    if (n == 0) { free(list); return; }

    // sort with priorities
    gSortPlayer = player;
    gSortNow    = currentTimeMillis();
    qsort(list, (size_t)n, sizeof(Chunk*), dirty_cmp);

    // rebuild up to 8 per frame
    int limit = n < 8 ? n : 8;
    for (int i = 0; i < limit; ++i) {
        Chunk_rebuild(list[i], 0);
        Chunk_rebuild(list[i], 1);
    }

    free(list);
}

static void draw_face_immediate(int x, int y, int z, int face) {
    float minX = (float)x,     maxX = (float)x + 1.0f;
    float minY = (float)y,     maxY = (float)y + 1.0f;
    float minZ = (float)z,     maxZ = (float)z + 1.0f;

    glBegin(GL_QUADS);
    switch (face) {
        case 0: glVertex3f(minX,minY,maxZ); glVertex3f(minX,minY,minZ); glVertex3f(maxX,minY,minZ); glVertex3f(maxX,minY,maxZ); break; // bottom
        case 1: glVertex3f(maxX,maxY,maxZ); glVertex3f(maxX,maxY,minZ); glVertex3f(minX,maxY,minZ); glVertex3f(minX,maxY,maxZ); break; // top
        case 2: glVertex3f(minX,maxY,minZ); glVertex3f(maxX,maxY,minZ); glVertex3f(maxX,minY,minZ); glVertex3f(minX,minY,minZ); break; // -Z
        case 3: glVertex3f(minX,maxY,maxZ); glVertex3f(minX,minY,maxZ); glVertex3f(maxX,minY,maxZ); glVertex3f(maxX,maxY,maxZ); break; // +Z
        case 4: glVertex3f(minX,maxY,maxZ); glVertex3f(minX,maxY,minZ); glVertex3f(minX,minY,minZ); glVertex3f(minX,minY,maxZ); break; // -X
        case 5: glVertex3f(maxX,minY,maxZ); glVertex3f(maxX,minY,minZ); glVertex3f(maxX,maxY,minZ); glVertex3f(maxX,maxY,maxZ); break; // +X
    }
    glEnd();
}

void LevelRenderer_renderHit(LevelRenderer* r, HitResult* h) {
    (void)r;
    if (!h) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    float a = (float)sin((double)currentTimeMillis() / 100.0) * 0.2f + 0.4f;
    a *= 0.5f;
    glColor4f(1.f, 1.f, 1.f, a);

    draw_face_immediate(h->x, h->y, h->z, h->f);

    glDisable(GL_BLEND);
}

void LevelRenderer_setDirty(const LevelRenderer* r, int minX, int minY, int minZ, int maxX, int maxY, int maxZ) {
    minX /= CHUNK_SIZE; minY /= CHUNK_SIZE; minZ /= CHUNK_SIZE;
    maxX /= CHUNK_SIZE; maxY /= CHUNK_SIZE; maxZ /= CHUNK_SIZE;

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (minZ < 0) minZ = 0;

    if (maxX >= r->chunkAmountX) maxX = r->chunkAmountX - 1;
    if (maxY >= r->chunkAmountY) maxY = r->chunkAmountY - 1;
    if (maxZ >= r->chunkAmountZ) maxZ = r->chunkAmountZ - 1;

    for (int x = minX; x <= maxX; ++x)
    for (int y = minY; y <= maxY; ++y)
    for (int z = minZ; z <= maxZ; ++z) {
        Chunk_setDirty(&r->chunks[(x + y * r->chunkAmountX) * r->chunkAmountZ + z]);
    }
}

void levelRenderer_tileChanged(LevelRenderer* r, int x, int y, int z) {
    LevelRenderer_setDirty(r, x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
}

void levelRenderer_lightColumnChanged(LevelRenderer* r, int x, int z, int minY, int maxY) {
    LevelRenderer_setDirty(r, x - 1, minY - 1, z - 1, x + 1, maxY + 1, z + 1);
}

void levelRenderer_allChanged(Level* level, LevelRenderer* r) {
    LevelRenderer_setDirty(r, 0, 0, 0, level->width, level->depth, level->height);
}
