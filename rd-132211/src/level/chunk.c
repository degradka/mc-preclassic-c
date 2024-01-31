#include "chunk.h"
#include "tile.h"
#include "tessellator.h"

extern Tessellator TESSELLATOR;

void Chunk_init(Chunk* chunk, Level* level, int minX, int minY, int minZ, int maxX, int maxY, int maxZ) {
    chunk->level = level;

    chunk->minX = minX;
    chunk->minY = minY;
    chunk->minZ = minZ;
    chunk->maxX = maxX;
    chunk->maxY = maxY;
    chunk->maxZ = maxZ;
}

void Chunk_render(Chunk* chunk) {
    // Setup tile rendering
    glEnable(GL_TEXTURE_2D);
    Tessellator_init(&TESSELLATOR);

    // For each tile in this chunk
    for (int x = chunk->minX; x < chunk->maxX; ++x) {
        for (int y = chunk->minY; y < chunk->maxY; ++y) {
            for (int z = chunk->minZ; z < chunk->maxZ; ++z) {
                // Is a tile at this location?
                if (Level_isTile(chunk->level, x, y, z)) {
                    // Render the tile
                    Tile_render(&rock, &TESSELLATOR, chunk->level, x, y, z);
                }
            }
        }
    }

    // Finish tile rendering
    Tessellator_flush(&TESSELLATOR);
    glDisable(GL_TEXTURE_2D);
}
