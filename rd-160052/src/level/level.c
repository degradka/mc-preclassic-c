// level/level.c — world storage, lighting columns, IO, and solid-cube queries

#include "level.h"
#include "levelrenderer.h"
#include "tile/tile.h"
#include "perlin.h"

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

    bool mapLoaded = Level_load(level);
    if (!mapLoaded) {
        Level_generateMap(level);
    }

    calcLightDepths(level, 0, 0, width, height);
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

void Level_generateMap(Level* level) {
    int w = level->width, h = level->height, d = level->depth;

    int* first  = (int*)malloc((size_t)w * h * sizeof(int));
    int* second = (int*)malloc((size_t)w * h * sizeof(int));
    int* cliff  = (int*)malloc((size_t)w * h * sizeof(int));
    int* rock   = (int*)malloc((size_t)w * h * sizeof(int));
    if (!first || !second || !cliff || !rock) {
        fprintf(stderr, "Perlin temp alloc failed\n");
        if (first)  free(first);
        if (second) free(second);
        if (cliff)  free(cliff);
        if (rock)   free(rock);
        return;
    }

    Perlin_read(w, h, 0, first);
    Perlin_read(w, h, 0, second);
    Perlin_read(w, h, 1, cliff);
    Perlin_read(w, h, 1, rock);

    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < d; ++y) {
            for (int z = 0; z < h; ++z) {
                int idx2D = x + z * w;
                int f = first [idx2D];
                int s = second[idx2D];

                // If cliff noise < 128, force to first height
                if (cliff[idx2D] < 128) s = f;

                int maxLevelHeight = ( (s > f ? s : f) ) / 8 + d / 3;
                int maxRockHeight  =  rock[idx2D] / 8 + d / 3;
                if (maxRockHeight > maxLevelHeight - 2) {
                    maxRockHeight = maxLevelHeight - 2;
                }

                int index = (y * h + z) * w + x;
                int id = 0;

                if (y == maxLevelHeight) id = 2; // grass
                if (y <  maxLevelHeight) id = 3; // dirt
                if (y <= maxRockHeight)  id = 1; // rock

                level->blocks[index] = (unsigned char)id;
            }
        }
    }

    free(first); free(second); free(cliff); free(rock);
}

bool Level_isLightBlocker(const Level* level, int x, int y, int z) {
    return Level_isSolidTile(level, x, y, z);
}

bool Level_isTile(const Level* level, int x, int y, int z) {
    return Level_getTile(level, x, y, z) > 0;
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

bool Level_load(Level* level) {
    gzFile f = gzopen("level.dat", "rb");
    if (!f) return false;

    size_t total = (size_t)level->width * level->height * level->depth;
    int readBytes = gzread(f, level->blocks, (unsigned)total);
    gzclose(f);

    if (readBytes != (int)total) {
        // corrupted/short file
        return false;
    }

    // notify renderer for a full refresh
    if (level->renderer) {
        levelRenderer_allChanged(level, level->renderer);
    }
    return true;
}

void Level_save(const Level* level) {
    gzFile f = gzopen("level.dat", "wb");
    if (!f) return;
    gzwrite(f, level->blocks, (unsigned)(level->width * level->height * level->depth));
    gzclose(f);
}

bool level_setTile(Level* level, int x, int y, int z, int type) {
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height) return false;

    int index = (y * level->height + z) * level->width + x;
    if (level->blocks[index] == (byte)type) return false;

    level->blocks[index] = (byte)type;
    calcLightDepths(level, x, z, 1, 1);
    if (level->renderer) levelRenderer_tileChanged(level->renderer, x, y, z);
    return true;
}

int Level_getTile(const Level* level, int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height)
        return 0;
    int index = (y * level->height + z) * level->width + x;
    return level->blocks[index];
}

bool Level_isLit(const Level* level, int x, int y, int z) {
    return (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height) ||
           (y >= level->lightDepths[x + z * level->width]);
}

void Level_onTick(Level* level) {
    // amount of tiles processed per tick
    int totalTiles = level->width * level->height * level->depth;
    int ticks = totalTiles / 400;
    if (ticks < 1) ticks = 1;

    for (int i = 0; i < ticks; ++i) {
        int x = rand() % level->width;
        int y = rand() % level->depth;
        int z = rand() % level->height;

        int id = Level_getTile(level, x, y, z);
        const Tile* t = (id >= 0 && id < 256) ? gTiles[id] : NULL;
        if (t && t->onTick) {
            t->onTick(t, level, x, y, z);
        }
    }
}