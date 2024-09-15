#include <stdio.h>

#include "levelrenderer.h"
#include "frustum.h"
#include "../timer.h"
#include "tile.h"
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

void LevelRenderer_init(LevelRenderer* renderer, Level* level) {
    // Calculate amount of chunks in the level
    renderer->chunkAmountX = level->width / CHUNK_SIZE;
    renderer->chunkAmountY = level->depth / CHUNK_SIZE;
    renderer->chunkAmountZ = level->height / CHUNK_SIZE;
    level->renderer = &renderer;

    // Create the chunk array
    renderer->chunks = (Chunk*)malloc(renderer->chunkAmountX * renderer->chunkAmountY * renderer->chunkAmountZ * sizeof(Chunk));
    if (!renderer->chunks) {
        fprintf(stderr, "Failed to allocate memory for chunks\n");
        exit(EXIT_FAILURE);
    }

    Tessellator_init(&renderer->tessellator);

    // Fill the level with chunks
    for (int x = 0; x < renderer->chunkAmountX; x++) {
        for (int y = 0; y < renderer->chunkAmountY; y++) {
            for (int z = 0; z < renderer->chunkAmountZ; z++) {
                // Calculate min bounds for the chunk
                int minChunkX = x * CHUNK_SIZE;
                int minChunkY = y * CHUNK_SIZE;
                int minChunkZ = z * CHUNK_SIZE;

                // Calculate max bounds for the chunk
                int maxChunkX = (x + 1) * CHUNK_SIZE;
                int maxChunkY = (y + 1) * CHUNK_SIZE;
                int maxChunkZ = (z + 1) * CHUNK_SIZE;

                // Check for chunk bounds out of level
                maxChunkX = MIN(level->width, maxChunkX);
                maxChunkY = MIN(level->depth, maxChunkY);
                maxChunkZ = MIN(level->height, maxChunkZ);

                // Create chunk based on bounds
                Chunk_init(&renderer->chunks[(x + y * renderer->chunkAmountX) * renderer->chunkAmountZ + z],
                           level, minChunkX, minChunkY, minChunkZ, maxChunkX, maxChunkY, maxChunkZ);
            }
        }
    }
}

void LevelRenderer_render(const LevelRenderer* renderer, int layer) {
    // Reset global chunk rebuild stats
    renderer->chunks->rebuiltThisFrame = 0;

    frustum_calculate();

    for (int i = 0; i < renderer->chunkAmountX * renderer->chunkAmountY * renderer->chunkAmountZ; ++i) {
        // Render if bounding box of chunk is in frustum
        if (frustum_cubeInAABB(&renderer->chunks[i].boundingBox)) {
            // Render chunk
            Chunk_render(&renderer->chunks[i], layer);
        }
    }
}

void LevelRenderer_destroy(LevelRenderer* renderer) {
    free(renderer->chunks);
}

void LevelRenderer_renderPick(Level* level, LevelRenderer* renderer) {
    float r = 3.0f;
    AABB* boundingBox;
    AABB_grow(boundingBox, r, r, r);
    int minX = boundingBox->minX;
    int maxX = boundingBox->maxX + 1.0f;
    int minY = boundingBox->minY;
    int maxY = boundingBox->maxY + 1.0f;
    int minZ = boundingBox->minZ;
    int maxZ = boundingBox->maxZ + 1.0f;
    glInitNames();

    for (int x = minX; x < maxX; x++) {
        glPushName(x);
        for (int y = minY; y < maxY; y++) {
            glPushName(y);
            for (int z = minZ; z < maxZ; z++) {
                glPushName(z);
                if (Level_isTile(level, x, y, z)) {
                    glPushName(0);

                    for (int i = 0; i < 6; i++) {
                        glPushName(i);
                        Tessellator_init(&renderer->tessellator);
                        Face_render(&renderer->tessellator, x, y, z, i);
                        Tessellator_flush(&renderer->tessellator);
                        glPopName();
                    }
                    glPopName();
                }
                glPopName();
            }
            glPopName();
        }
        glPopName();
    }
}

void LevelRenderer_renderHit(LevelRenderer* renderer, HitResult* h) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glColor4f(1.0f, 1.0f, 1.0f, (float)sin((double)currentTimeMillis() / 100.0) * 0.2f + 0.4f);
    Tessellator_init(&renderer->tessellator);
    Face_render(&renderer->tessellator, h->x, h->y, h->z, h->f);
    Tessellator_flush(&renderer->tessellator);
    glDisable(GL_BLEND);

}

void LevelRenderer_setDirty(const LevelRenderer* renderer, int minX, int minY, int minZ, int maxX, int maxY, int maxZ) {
    // To chunk coordinates
    minX /= CHUNK_SIZE;
    minY /= CHUNK_SIZE;
    minZ /= CHUNK_SIZE;
    maxX /= CHUNK_SIZE;
    maxY /= CHUNK_SIZE;
    maxZ /= CHUNK_SIZE;

    // Minimum limit
    if (minX < 0) {
        minX = 0;
    }
    if (minY < 0) {
        minY = 0;
    }
    if (minZ < 0) {
        minZ = 0;
    }

    // Maximum limit
    if (maxX >= renderer->chunkAmountX) {
        maxX = renderer->chunkAmountX - 1;
    }
    if (maxY >= renderer->chunkAmountY) {
        maxY = renderer->chunkAmountY - 1;
    }
    if (maxZ >= renderer->chunkAmountZ) {
        maxZ = renderer->chunkAmountZ - 1;
    }

    // Mark all chunks as dirty
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            for (int z = minZ; z <= maxZ; ++z) {
                // Get chunk at this position
                Chunk_setDirty(&renderer->chunks[(x + y * renderer->chunkAmountX) * renderer->chunkAmountZ + z]);
            }
        }
    }
}

void levelRenderer_tileChanged(LevelRenderer* renderer, int x, int y, int z) {
    LevelRenderer_setDirty(renderer, x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
}

void levelRenderer_lightColumnChanged(LevelRenderer* renderer, int x, int z, int minY, int maxY) {
    LevelRenderer_setDirty(renderer, x - 1, minY - 1, z - 1, x + 1, maxY + 1, z + 1);
}

void levelRenderer_allChanged(Level* level, LevelRenderer* renderer) {
    LevelRenderer_setDirty(renderer, 0, 0, 0, level->width, level->depth, level->height);
}
