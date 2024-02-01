#ifndef CHUNK_H
#define CHUNK_H

#include <stdbool.h>

#include "level.h"
#include "tessellator.h"

typedef struct {
    Level* level;
    int texture;

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
