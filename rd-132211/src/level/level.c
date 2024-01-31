#include <stdio.h>

#include "level.h"

void Level_init(Level* level, int width, int height, int depth) {
    level->width = width;
    level->height = height;
    level->depth = depth;

    // Create level with tiles
    level->blocks = (byte*)malloc(width * height * depth * sizeof(byte));
    if (!level->blocks) {
        fprintf(stderr, "Failed to allocate memory for level blocks\n");
        exit(EXIT_FAILURE);
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < depth; y++) {
            for (int z = 0; z < height; z++) {
                // Calculate index from x, y, and z
                int index = (y * level->height + z) * level->width + x;

                // Set tile based on location
                level->blocks[index] = (byte)(y < depth / 2 ? 1 : 0);
            }
        }
    }
}

bool Level_isTile(const Level* level, int x, int y, int z) {
    // Is location out of the level?
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height) {
        return false;
    }

    // Calculate index from x, y, and z
    int index = (y * level->height + z) * level->width + x;

    // Return true if there is a tile at this location
    return level->blocks[index] != 0;
}

bool Level_isSolidTile(const Level* level, int x, int y, int z) {
    return Level_isTile(level, x, y, z);
}

void Level_destroy(Level* level) {
    free(level->blocks);
}
