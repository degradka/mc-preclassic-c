#include <stdio.h>

#include "levelrenderer.h"
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

void LevelRenderer_render(const LevelRenderer* renderer) {
    for (int i = 0; i < renderer->chunkAmountX * renderer->chunkAmountY * renderer->chunkAmountZ; ++i) {
        Chunk_render(&renderer->chunks[i]);
    }
}

void LevelRenderer_destroy(LevelRenderer* renderer) {
    free(renderer->chunks);
}
