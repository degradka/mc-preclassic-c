#include "chunk.h"
#include "tile.h"
#include "tessellator.h"
#include "../textures.h"

extern Tessellator TESSELLATOR;

void Chunk_init(Chunk* chunk, Level* level, int minX, int minY, int minZ, int maxX, int maxY, int maxZ) {
    chunk->level = level;

    chunk->texture = loadTexture("resources/terrain.png", GL_NEAREST);

    chunk->minX = minX;
    chunk->minY = minY;
    chunk->minZ = minZ;
    chunk->maxX = maxX;
    chunk->maxY = maxY;
    chunk->maxZ = maxZ;

    chunk->lists = glGenLists(1);
    chunk->dirty = true;
}

void Chunk_rebuild(Chunk* chunk) {
    // Set the chunk as not dirty
    chunk->dirty = false;

    // Setup tile rendering
    glNewList(chunk->lists, GL_COMPILE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, chunk->texture);
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
    glEndList();
}

void Chunk_render(Chunk* chunk) {
    // Rebuild chunk if dirty
    if (chunk->dirty) {
        Chunk_rebuild(chunk);
    }

    // Call display list ID to render the chunk
    glCallList(chunk->lists);
}