// level/levelrenderer.h

#ifndef LEVELRENDERER_H_
#define LEVELRENDERER_H_

#include <algorithm>
#include "level.h"
#include "chunk.h"

class LevelRenderer {
public:
    LevelRenderer(Level* level);
    ~LevelRenderer();
    void render();

private:
    Chunk** chunks;
    int chunkAmountX;
    int chunkAmountY;
    int chunkAmountZ;
};

#endif // LEVELRENDERER_H_