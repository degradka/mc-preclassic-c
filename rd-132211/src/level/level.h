// level.h

#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../phys/aabb.h"
#include <math.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef unsigned char byte;

typedef struct {
    int width;
    int height;
    int depth;
    byte* blocks;
    int* lightDepths;

    struct LevelRenderer* renderer;
} Level;

typedef struct {
    int size;
    int capacity;
    AABB* aabbs;
} ArrayList_AABB;

ArrayList_AABB Level_getCubes(const Level* level, const AABB* boundingBox);

void calcLightDepths(Level* level, int minX, int minZ, int maxX, int maxZ);
void Level_init(Level* level, int width, int height, int depth);
bool Level_isTile(const Level* level, int x, int y, int z);
bool Level_isSolidTile(const Level* level, int x, int y, int z);
void Level_destroy(Level* level);
float Level_getBrightness(const Level* level, int x, int y, int z);
bool Level_isLightBlocker(const Level* level, int x, int y, int z);
void Level_load(Level* level);
void Level_save(const Level* level);
void level_setTile(Level* level, int x, int y, int z, int type);

ArrayList_AABB Level_getCubes(const Level* level, const AABB* boundingBox);

#endif  // LEVEL_H
