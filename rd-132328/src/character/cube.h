// character/cube.h — textured box model part

#ifndef CHARACTER_CUBE_H
#define CHARACTER_CUBE_H

#include <GL/glew.h>
#include "../level/tessellator.h"

typedef struct Cube {
    int u, v;

    float x0, y0, z0;
    float x1, y1, z1;

    float px, py, pz;
} Cube;

void Cube_init(Cube* c, int u, int v);
void Cube_addBox(Cube* c, float x, float y, float z, int w, int h, int d);
void Cube_setPosition(Cube* c, float px, float py, float pz);

void Cube_render(const Cube* c, Tessellator* t);

#endif /* CHARACTER_CUBE_H */
