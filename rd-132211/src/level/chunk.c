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
    chunk->dirty = true;

    // Create bounding box object of chunk
    chunk->boundingBox = AABB_create(minX, minY, minZ, maxX, maxY, maxZ);

    // Generate lists id
    chunk->lists = glGenLists(2);
}

void Chunk_rebuild(Chunk* chunk, int layer) {
    if (chunk->rebuiltThisFrame == 2) {
        // Rebuild limit reached for this frame
        return;
    }

    // Update global stats
    chunk->updates++;
    chunk->rebuiltThisFrame++;

    // Set the chunk as not dirty
    chunk->dirty = false;

    // Setup tile rendering
    glNewList(chunk->lists + layer, GL_COMPILE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, chunk->texture);
    Tessellator_init(&TESSELLATOR);

    // For each tile in this chunk
    for (int x = chunk->minX; x < chunk->maxX; ++x) {
        for (int y = chunk->minY; y < chunk->maxY; ++y) {
            for (int z = chunk->minZ; z < chunk->maxZ; ++z) {
                // Is a tile at this location?
                if (Level_isTile(chunk->level, x, y, z)) {
                    //int id = y != chunk->level->depth * 2 / 3;

                    if (y > chunk->level->depth - 40 && Level_getBrightness(chunk->level, x, y, z) == 1.0F) {
                        Tile_render(&grass, &TESSELLATOR, chunk->level, layer, x, y, z);
                    } else {
                        Tile_render(&rock, &TESSELLATOR, chunk->level, layer, x, y, z);
                    }
                }
            }
        }
    }

    // Finish tile rendering
    Tessellator_flush(&TESSELLATOR);
    glDisable(GL_TEXTURE_2D);
    glEndList();
}

void Chunk_render(Chunk* chunk, int layer) {
    // Rebuild chunk if dirty
    if (chunk->dirty) {
        Chunk_rebuild(chunk, 0);
        Chunk_rebuild(chunk, 1);
    }

    // Call display list ID to render the chunk
    glCallList(chunk->lists + layer);
}

void Chunk_setDirty(Chunk* chunk) {
    chunk->dirty = true;
}
