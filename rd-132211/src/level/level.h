#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned char byte;

typedef struct {
    int width;
    int height;
    int depth;
    byte* blocks;
} Level;

void Level_init(Level* level, int width, int height, int depth);
bool Level_isTile(const Level* level, int x, int y, int z);
bool Level_isSolidTile(const Level* level, int x, int y, int z);
void Level_destroy(Level* level);

#endif  // LEVEL_H