// level/chunk.h — chunk display lists, rebuild & render

#ifndef CHUNK_H
#define CHUNK_H

#include <stdbool.h>
#include "level.h"
#include "tessellator.h"
#include "phys/aabb.h"

struct Level; typedef struct Level Level;

typedef struct Chunk {
    Level* level;
    int    texture;

    AABB boundingBox;

    int minX, minY, minZ;
    int maxX, maxY, maxZ;

    int  lists;   // base display list id (0:bright, 1:shadow)
    bool dirty;
} Chunk;

void Chunk_init(Chunk* chunk, Level* level, int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
void Chunk_rebuild(Chunk* chunk, int layer);
void Chunk_render(Chunk* chunk, int layer);
void Chunk_setDirty(Chunk* chunk);

#endif  // CHUNK_H