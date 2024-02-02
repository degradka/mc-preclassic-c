#include "levelrenderer.h"

#define CHUNK_SIZE 16

LevelRenderer::LevelRenderer(Level* level) : chunks(nullptr), chunkAmountX(0), chunkAmountY(0), chunkAmountZ(0) {
    // Calculate amount of chunks for the level
    chunkAmountX = level->width / CHUNK_SIZE;
    chunkAmountY = level->depth / CHUNK_SIZE;
    chunkAmountZ = level->height / CHUNK_SIZE;

    // Create chunk array
    chunks = new Chunk*[chunkAmountX * chunkAmountY * chunkAmountZ];

    // Fill level with chunks
    for (int x = 0; x < chunkAmountX; ++x) {
        for (int y = 0; y < chunkAmountY; ++y) {
            for (int z = 0; z < chunkAmountZ; ++z) {
                // Calculate min bounds for chunk
                int minChunkX = x * CHUNK_SIZE;
                int minChunkY = y * CHUNK_SIZE;
                int minChunkZ = z * CHUNK_SIZE;

                // Calculate max bounds for chunk
                int maxChunkX = (x + 1) * CHUNK_SIZE;
                int maxChunkY = (y + 1) * CHUNK_SIZE;
                int maxChunkZ = (z + 1) * CHUNK_SIZE;

                // Check for chunk bounds out of level
                maxChunkX = std::min(level->width, maxChunkX);
                maxChunkY = std::min(level->depth, maxChunkY);
                maxChunkZ = std::min(level->height, maxChunkZ);

                // Create chunk based on bounds
                Chunk* chunk = new Chunk(level, minChunkX, minChunkY, minChunkZ, maxChunkX, maxChunkY, maxChunkZ);
                chunks[(x + y * chunkAmountX) * chunkAmountZ + z] = chunk;
            }
        }
    }
}

LevelRenderer::~LevelRenderer() {
    // Delete allocated chunks
    for (int i = 0; i < chunkAmountX * chunkAmountY * chunkAmountZ; ++i) {
        delete chunks[i];
    }
    delete[] chunks;
}

void LevelRenderer::render() {
    for (int i = 0; i < chunkAmountX * chunkAmountY * chunkAmountZ; ++i) {
        chunks[i]->render();
    }
}