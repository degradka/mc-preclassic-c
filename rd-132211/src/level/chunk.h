#ifndef CHUNK_H
#define CHUNK_H

#include "level.h"
#include "tessellator.h"

typedef struct {
    Level* level;

    int minX, minY, minZ;
    int maxX, maxY, maxZ;
} Chunk;

void Chunk_init(Chunk* chunk, Level* level, int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
void Chunk_render(Chunk* chunk);

#endif  // CHUNK_H
