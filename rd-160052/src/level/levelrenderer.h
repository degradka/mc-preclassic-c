// level/levelrenderer.h — chunk grid, frustum culling, dirty-marking, and hit highlight

#ifndef LEVELRENDERER_H
#define LEVELRENDERER_H

#include "../hitresult.h"
#include "tessellator.h"

struct Level;     typedef struct Level Level;
struct Chunk;     typedef struct Chunk Chunk;
struct HitResult; // forward

struct Level;   typedef struct Level Level;
struct Chunk;   typedef struct Chunk Chunk;
struct Player;  typedef struct Player Player;

#define CHUNK_SIZE 16

typedef struct LevelRenderer {
    Chunk*      chunks;
    int         chunkAmountX, chunkAmountY, chunkAmountZ;
    Tessellator tessellator;
} LevelRenderer;

void LevelRenderer_init(LevelRenderer* renderer, Level* level);
void LevelRenderer_render(const LevelRenderer* renderer, int layer);
void LevelRenderer_destroy(LevelRenderer* renderer);

void LevelRenderer_setDirty(const LevelRenderer* renderer, int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
void levelRenderer_tileChanged(LevelRenderer* renderer, int x, int y, int z);
void levelRenderer_lightColumnChanged(LevelRenderer* renderer, int x, int z, int minY, int maxY);
void levelRenderer_allChanged(Level* level, LevelRenderer* renderer);

void LevelRenderer_renderHit(LevelRenderer* renderer, struct HitResult* h);

void LevelRenderer_updateDirtyChunks(LevelRenderer* r, const Player* player);

#endif  // LEVELRENDERER_H
