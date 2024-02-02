// level/level.cpp

#include "level.h"

Level::Level(int width, int height, int depth) : width(width), height(height), depth(depth) {
    // Create level with tiles
    this->blocks = new signed char[width * height * depth];
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < depth; y++) {
            for (int z = 0; z < height; z++) {
                // Calculate index from x, y, and z
                int index = (y * this->height + z) * this->width + x;

                // Set tile based on location
                this->blocks[index] = (signed char)(y < depth / 2 ? 1 : 0);
            }
        }
    }
}

bool Level::isTile(int x, int y, int z) {
    // Is location out of the level?
    if (x < 0 || y < 0 || z < 0 || x >= this->width || y >= this->depth || z >= this->height) {
        return false;
    }

    // Calculate index from x, y, and z
    int index = (y * this->height + z) * this->width + x;

    // Return true if there is a tile at this location
    return this->blocks[index] != 0;
}

bool Level::isSolidTile(int x, int y, int z) {
    return isTile(x, y, z);
}