#ifndef CHUNK_H
#define CHUNK_H

#include <stdbool.h>

#include "level.h"
#include "tessellator.h"
#include "phys/aabb.h"

typedef struct {
    int rebuiltThisFrame;
    int updates;

    Level* level;
    int texture;

    AABB boundingBox;

    int minX, minY, minZ;
    int maxX, maxY, maxZ;

    int lists;
    bool dirty;
} Chunk;

void Chunk_init(Chunk* chunk, Level* level, int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
void Chunk_rebuild(Chunk* chunk, int layer);
void Chunk_render(Chunk* chunk, int layer);
void Chunk_setDirty(Chunk* chunk);

#endif  // CHUNK_H
