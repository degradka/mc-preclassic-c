// character/cube.h

#ifndef CUBE_H
#define CUBE_H

#include "polygon.h"

typedef struct {
    Polygon polys[6];
    float   x, y, z;
    float   xRot, yRot, zRot;
    int     texOffX, texOffY;
} Cube;

void  Cube_init(Cube* c, int texOffX, int texOffY);
Cube* Cube_addBox(Cube* c, float ox, float oy, float oz, int w, int h, int d);
void  Cube_setPos(Cube* c, float x, float y, float z);
void  Cube_render(const Cube* c);

#endif
