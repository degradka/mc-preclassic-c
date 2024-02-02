// level/chunk.h

#ifndef CHUNK_H_
#define CHUNK_H_

#include "level.h"
#include "tessellator.h"

class Chunk {
public:
    Chunk(Level* level, int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
    ~Chunk();
    void render();

private:
    Level* level;
    int minX, minY, minZ;
    int maxX, maxY, maxZ;
    Tessellator* tessellator;
};

#endif // CHUNK_H_
