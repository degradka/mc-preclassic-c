// level.c — world storage, lighting columns, IO, and solid-cube queries

#include "level.h"
#include "levelrenderer.h"
#include <zlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void Level_init(Level* level, int width, int height, int depth) {
    level->width = width;
    level->height = height;
    level->depth = depth;
    level->renderer = NULL;

    level->blocks = (byte*)malloc((size_t)width * height * depth);
    level->lightDepths = (int*)malloc((size_t)width * height * sizeof(int));
    if (!level->blocks || !level->lightDepths) {
        fprintf(stderr, "Failed to allocate level memory\n");
        exit(EXIT_FAILURE);
    }

    // default terrain: rock up to 2/3 depth, air above (Java parity)
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < depth; y++) {
            for (int z = 0; z < height; z++) {
                int index = (y * level->height + z) * level->width + x;
                level->blocks[index] = (byte)(y <= (depth * 2 / 3) ? 1 : 0);
            }
        }
    }

    // load saved blocks (overwrites defaults if file exists)
    Level_load(level);
}

void calcLightDepths(Level* level, int minX, int minZ, int maxX, int maxZ) {
    for (int x = minX; x < minX + maxX; x++) {
        for (int z = minZ; z < minZ + maxZ; z++) {
            int prev = level->lightDepths[x + z * level->width];
            int d = level->depth - 1;
            while (d > 0 && !Level_isLightBlocker(level, x, d, z)) d--;
            level->lightDepths[x + z * level->width] = d;

            if (prev != d && level->renderer) {
                int ylMin = prev < d ? prev : d;
                int ylMax = prev > d ? prev : d;
                levelRenderer_lightColumnChanged(level->renderer, x, z, ylMin, ylMax);
            }
        }
    }
}

bool Level_isLightBlocker(const Level* level, int x, int y, int z) {
    return Level_isSolidTile(level, x, y, z);
}

float Level_getBrightness(const Level* level, int x, int y, int z) {
    const float dark = 0.8f, light = 1.0f;
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height)
        return light;
    return (y < level->lightDepths[x + z * level->width]) ? dark : light;
}

bool Level_isTile(const Level* level, int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height)
        return false;
    int index = (y * level->height + z) * level->width + x;
    return level->blocks[index] != 0;
}

bool Level_isSolidTile(const Level* level, int x, int y, int z) {
    return Level_isTile(level, x, y, z);
}

void Level_destroy(Level* level) {
    free(level->blocks);
    free(level->lightDepths);
}

ArrayList_AABB Level_getCubes(const Level* level, const AABB* bb) {
    ArrayList_AABB out = { .size = 0, .capacity = 10 };
    out.aabbs = (AABB*)malloc((size_t)out.capacity * sizeof(AABB));

    int minX = (int)floor(bb->minX) - 1;
    int maxX = (int)ceil (bb->maxX) + 1;
    int minY = (int)floor(bb->minY) - 1;
    int maxY = (int)ceil (bb->maxY) + 1;
    int minZ = (int)floor(bb->minZ) - 1;
    int maxZ = (int)ceil (bb->maxZ) + 1;

    minX = MAX(0, minX); minY = MAX(0, minY); minZ = MAX(0, minZ);
    maxX = MIN(level->width,  maxX);
    maxY = MIN(level->depth,  maxY);
    maxZ = MIN(level->height, maxZ);

    for (int x = minX; x < maxX; x++)
    for (int y = minY; y < maxY; y++)
    for (int z = minZ; z < maxZ; z++) {
        if (!Level_isSolidTile(level, x, y, z)) continue;
        if (out.size == out.capacity) {
            out.capacity *= 2;
            out.aabbs = (AABB*)realloc(out.aabbs, (size_t)out.capacity * sizeof(AABB));
            if (!out.aabbs) { fprintf(stderr, "AABB realloc failed\n"); exit(EXIT_FAILURE); }
        }
        out.aabbs[out.size++] = AABB_create(x, y, z, x + 1, y + 1, z + 1);
    }
    return out;
}

void Level_load(Level* level) {
    gzFile f = gzopen("level.dat", "rb");
    if (!f) return;
    gzread(f, level->blocks, (unsigned)(level->width * level->height * level->depth));
    gzclose(f);
}

void Level_save(const Level* level) {
    gzFile f = gzopen("level.dat", "wb");
    if (!f) return;
    gzwrite(f, level->blocks, (unsigned)(level->width * level->height * level->depth));
    gzclose(f);
}

void level_setTile(Level* level, int x, int y, int z, int type) {
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height) return;
    level->blocks[(y * level->height + z) * level->width + x] = (byte)type;
    calcLightDepths(level, x, z, 1, 1);
    if (level->renderer) levelRenderer_tileChanged(level->renderer, x, y, z);
}
