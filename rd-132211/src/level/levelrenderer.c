#include <stdio.h>

#include "levelrenderer.h"
#include "frustum.h"
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

void LevelRenderer_init(LevelRenderer* renderer, Level* level) {
    // Calculate amount of chunks in the level
    renderer->chunkAmountX = level->width / CHUNK_SIZE;
    renderer->chunkAmountY = level->depth / CHUNK_SIZE;
    renderer->chunkAmountZ = level->height / CHUNK_SIZE;

    // Create the chunk array
    renderer->chunks = (Chunk*)malloc(renderer->chunkAmountX * renderer->chunkAmountY * renderer->chunkAmountZ * sizeof(Chunk));
    if (!renderer->chunks) {
        fprintf(stderr, "Failed to allocate memory for chunks\n");
        exit(EXIT_FAILURE);
    }

    // Fill the level with chunks
    for (int x = 0; x < renderer->chunkAmountX; ++x) {
        for (int y = 0; y < renderer->chunkAmountY; ++y) {
            for (int z = 0; z < renderer->chunkAmountZ; ++z) {
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

// TODO: Frustum doesn't work properly???????
void LevelRenderer_render(const LevelRenderer* renderer, int layer) {
    //Frustum* frustum = Frustum_create();

    for (int i = 0; i < renderer->chunkAmountX * renderer->chunkAmountY * renderer->chunkAmountZ; ++i) {
        // Render if bounding box of chunk is in frustum
        //if (Frustum_aabbInFrustum(frustum, &renderer->chunks[i].boundingBox)) {
            // Render chunk
        Chunk_render(&renderer->chunks[i], layer);
        //}
    }
}

void LevelRenderer_destroy(LevelRenderer* renderer) {
    free(renderer->chunks);
}

void LevelRenderer_setDirty(const LevelRenderer* renderer, int minX, int minY, int minZ, int maxX, int maxY, int maxZ) {
    // To chunk coordinates
    minX /= CHUNK_SIZE;
    minY /= CHUNK_SIZE;
    minZ /= CHUNK_SIZE;
    maxX /= CHUNK_SIZE;
    maxY /= CHUNK_SIZE;
    maxZ /= CHUNK_SIZE;

    // Minimum and maximum y
    minY = MIN(0, minY);
    maxY = MIN(15, maxY);

    // Mark all chunks as dirty
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            for (int z = minZ; z <= maxZ; z++) {
                // Get chunk at this position
                Chunk_setDirty(&renderer->chunks[(x + y * renderer->chunkAmountX) * renderer->chunkAmountZ + z]);
            }
        }
    }
}