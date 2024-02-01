// level.c

#include "level.h"

void Level_init(Level* level, int width, int height, int depth) {
    level->width = width;
    level->height = height;
    level->depth = depth;

    /*
    FILE* levelFile = fopen("level.dat", "rb");
    if (levelFile) {
        fclose(levelFile);
        Level_load(level);
    } else {
        */
    // Create level with tiles
    level->blocks = (byte*)malloc(width * height * depth * sizeof(byte));
    level->lightDepths = (int*)malloc(width * height * sizeof(int));
    if (!level->blocks || !level->lightDepths) {
        fprintf(stderr, "Failed to allocate memory for level blocks or lightDepths\n");
        exit(EXIT_FAILURE);
    }

    // Fill level with tiles
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

    // Initialize lightDepths array
    calcLightDepths(level, 0, 0, width, height);

    // Generate caves
    for (int i = 0; i < 1000; i++) {
        int caveSize = (int) (rand() % 7) + 1;

        int caveX = rand() % level->width;
        int caveY = rand() % level->depth;
        int caveZ = rand() % level->height;

        // Grow cave
        for (int radius = 0; radius < caveSize; radius++) {
            if (radius == 0) {
                    // Avoid division by zero
                continue;
            }

            for (int sphere = 0; sphere < 1000; sphere++) {
                int offsetX = rand() % (radius * 2) - radius;
                int offsetY = rand() % (radius * 2) - radius;
                int offsetZ = rand() % (radius * 2) - radius;

                double distance = pow(offsetX, 2) + pow(offsetY, 2) + pow(offsetZ, 2);
                if (distance > radius * radius) {
                    continue;
                }

                int tileX = caveX + offsetX;
                int tileY = caveY + offsetY;
                int tileZ = caveZ + offsetZ;

                // Check if tile is within the level bounds
                if (tileX >= 0 && tileY >= 0 && tileZ >= 0 && tileX < level->width - 1 && tileY < level->depth && tileZ < level->height - 1) {

                    // Calculate index from x, y, and z
                    int index = (tileY * level->height + tileZ) * level->width + tileX;

                    // Fill level with tiles
                    level->blocks[index] = (byte)0;
                }
            }
        }
    }
}

void calcLightDepths(Level* level, int minX, int minZ, int maxX, int maxZ) {
    // For each x/z position in level
    for (int x = minX; x < minX + maxX; x++) {
        for (int z = minZ; z < minZ + maxZ; z++) {

            // Get previous light depth value
            int prevDepth = level->lightDepths[x + z * level->width];

            // Calculate new light depth
            int depth = level->depth - 1;
            while (depth > 0 && !Level_isLightBlocker(level, x, depth, z)) {
                depth--;
            }

            // Set new light depth
            level->lightDepths[x + z * level->width] = depth;
        }
    }
}

bool Level_isLightBlocker(const Level* level, int x, int y, int z) {
    return Level_isSolidTile(level, x, y, z);
}

float Level_getBrightness(const Level* level, int x, int y, int z) {
    // Define brightness
    float dark = 0.8F;
    float light = 1.0F;

    // Is light tile
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height) {
        return light;
    }

    // Is dark tile
    if (y < level->lightDepths[x + z * level->width]) {
        return dark;
    }

    // Unknown brightness
    return light;
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
    free(level->lightDepths);
}

ArrayList_AABB Level_getCubes(const Level* level, const AABB* boundingBox) {
    ArrayList_AABB result;
    result.size = 0;
    result.capacity = 10;
    result.aabbs = (AABB*)malloc(result.capacity * sizeof(AABB));

    int minX = (int)(floor(boundingBox->minX) - 1);
    int maxX = (int)(ceil(boundingBox->maxX) + 1);
    int minY = (int)(floor(boundingBox->minY) - 1);
    int maxY = (int)(ceil(boundingBox->maxY) + 1);
    int minZ = (int)(floor(boundingBox->minZ) - 1);
    int maxZ = (int)(ceil(boundingBox->maxZ) + 1);

    // Minimum level position
    minX = MAX(0, minX);
    minY = MAX(0, minY);
    minZ = MAX(0, minZ);

    // Maximum level position
    maxX = MIN(level->width, maxX);
    maxY = MIN(level->depth, maxY);
    maxZ = MIN(level->height, maxZ);

    // Include all surrounding tiles
    for (int x = minX; x < maxX; x++) {
        for (int y = minY; y < maxY; y++) {
            for (int z = minZ; z < maxZ; z++) {
                if (Level_isSolidTile(level, x, y, z)) {
                    if (result.size == result.capacity) {
                        // Resize the array if needed
                        result.capacity *= 2;
                        result.aabbs = (AABB*)realloc(result.aabbs, result.capacity * sizeof(AABB));
                        if (!result.aabbs) {
                            fprintf(stderr, "Failed to reallocate memory for AABBs\n");
                            exit(EXIT_FAILURE);
                        }
                    }

                    // Add the new AABB to the array
                    result.aabbs[result.size++] = AABB_create(x, y, z, x + 1, y + 1, z + 1);
                }
            }
        }
    }
    return result;
}

// TODO: Level loading gives segfault
void Level_load(Level* level) {
    FILE* file = fopen("level.dat", "rb");
    if (file) {
        fread(level->blocks, sizeof(byte), level->width * level->height * level->depth, file);
        fclose(file);

        // Recalculate light depths after loading
        calcLightDepths(level, 0, 0, level->width, level->height);
    } else {
        fprintf(stderr, "Failed to open level.dat for reading\n");
    }
}

void Level_save(const Level* level) {
    FILE* file = fopen("level.dat", "wb");
    if (file) {
        fwrite(level->blocks, sizeof(byte), level->width * level->height * level->depth, file);
        fclose(file);
    }
}