// level/chunk.cpp

#include "chunk.h"
#include "tile.h"

Chunk::Chunk(Level* level, int minX, int minY, int minZ, int maxX, int maxY, int maxZ)
    : level(level), minX(minX), minY(minY), minZ(minZ), maxX(maxX), maxY(maxY), maxZ(maxZ), tessellator(nullptr) {
    tessellator = new Tessellator();
}

Chunk::~Chunk() {
    delete tessellator;
}

void Chunk::render() {
    // Setup tile rendering
    glEnable(GL_TEXTURE_2D);
    tessellator->init();

    // For each tile in this chunk
    for (int x = minX; x < maxX; ++x) {
        for (int y = minY; y < maxY; ++y) {
            for (int z = minZ; z < maxZ; ++z) {

                // Is a tile at this location?
                if (level->isTile(x, y, z)) {
                    // Render the tile
                    Tile::rock.render(tessellator, level, x, y, z);
                }
            }
        }
    }

    // Finish tile rendering
    tessellator->flush();
    glDisable(GL_TEXTURE_2D);
}