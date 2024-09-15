#ifndef LEVELRENDERER_H
#define LEVELRENDERER_H

#include "level.h"
#include "chunk.h"
#include "../hitresult.h"
#include "tessellator.h"

#define CHUNK_SIZE 16

typedef struct {
    Chunk* chunks;
    int chunkAmountX;
    int chunkAmountY;
    int chunkAmountZ;
    Tessellator tessellator;
} LevelRenderer;

void LevelRenderer_init(LevelRenderer* renderer, Level* level);
void LevelRenderer_render(const LevelRenderer* renderer, int layer);
void levelRenderer_tileChanged(LevelRenderer* renderer, int x, int y, int z);
void levelRenderer_lightColumnChanged(LevelRenderer* renderer, int x, int z, int minY, int maxY);
void levelRenderer_allChanged(Level* level, LevelRenderer* renderer);
void LevelRenderer_destroy(LevelRenderer* renderer);
void LevelRenderer_setDirty(const LevelRenderer* renderer, int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
void LevelRenderer_renderPick(Level* level, LevelRenderer* renderer);
void LevelRenderer_renderHit(LevelRenderer* renderer, HitResult* h);

#endif  // LEVELRENDERER_H
