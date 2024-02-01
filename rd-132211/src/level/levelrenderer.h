#ifndef LEVELRENDERER_H
#define LEVELRENDERER_H

#include "level.h"
#include "chunk.h"

#define CHUNK_SIZE 16

typedef struct {
    Chunk* chunks;
    int chunkAmountX;
    int chunkAmountY;
    int chunkAmountZ;
} LevelRenderer;

void LevelRenderer_init(LevelRenderer* renderer, Level* level);
void LevelRenderer_render(const LevelRenderer* renderer, int layer);
void LevelRenderer_destroy(LevelRenderer* renderer);
void LevelRenderer_setDirty(const LevelRenderer* renderer, int minX, int minY, int minZ, int maxX, int maxY, int maxZ);

#endif  // LEVELRENDERER_H
