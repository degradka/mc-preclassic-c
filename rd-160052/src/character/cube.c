// character/cube.c

#include "cube.h"
#include <GL/glew.h>
#include <math.h>
#include "common.h"

void Cube_init(Cube* c, int tx, int ty) {
    c->texOffX = tx; c->texOffY = ty;
    c->x = c->y = c->z = 0.0f;
    c->xRot = c->yRot = c->zRot = 0.0f;
}

Cube* Cube_addBox(Cube* c, float ox, float oy, float oz, int w, int h, int d) {
    const float x = ox + w, y = oy + h, z = oz + d;

    const Vertex b1 = Vertex_make(ox, oy, oz, 0, 0);
    const Vertex b2 = Vertex_make(x,  oy, oz, 0, 8);
    const Vertex b3 = Vertex_make(ox, oy, z,  0, 0);
    const Vertex b4 = Vertex_make(x,  oy, z,  0, 8);

    const Vertex t1 = Vertex_make(x,  y,  z,  8, 8);
    const Vertex t2 = Vertex_make(ox, y,  z,  8, 0);
    const Vertex t3 = Vertex_make(x,  y,  oz, 8, 8);
    const Vertex t4 = Vertex_make(ox, y,  oz, 8, 0);

    // right
    Polygon_init_uv(&c->polys[0], b4, b2, t3, t1,
        c->texOffX + d + w,     c->texOffY + d,
        c->texOffX + d + w + d, c->texOffY + d + h);

    // left
    Polygon_init_uv(&c->polys[1], b1, b3, t2, t4,
        c->texOffX,              c->texOffY + d,
        c->texOffX + d,          c->texOffY + d + h);

    // bottom
    Polygon_init_uv(&c->polys[2], b4, b3, b1, b2,
        c->texOffX + d,          c->texOffY,
        c->texOffX + d + w,      c->texOffY + d);

    // top
    Polygon_init_uv(&c->polys[3], t3, t4, t2, t1,
        c->texOffX + d + w,      c->texOffY,
        c->texOffX + d + w + w,  c->texOffY + d);

    // front
    Polygon_init_uv(&c->polys[4], b2, b1, t4, t3,
        c->texOffX + d,          c->texOffY + d,
        c->texOffX + d + w,      c->texOffY + d + h);

    // back
    Polygon_init_uv(&c->polys[5], b3, b4, t1, t2,
        c->texOffX + d + w + d,      c->texOffY + d,
        c->texOffX + d + w + d + w,  c->texOffY + d + h);

    return c;
}

void Cube_setPos(Cube* c, float x, float y, float z) { c->x = x; c->y = y; c->z = z; }

void Cube_render(const Cube* c) {
    glPushMatrix();
    glTranslatef(c->x, c->y, c->z);
    glRotated(c->zRot * 180.0 / M_PI, 0, 0, 1);
    glRotated(c->yRot * 180.0 / M_PI, 0, 1, 0);
    glRotated(c->xRot * 180.0 / M_PI, 1, 0, 0);

    glBegin(GL_QUADS);
    for (int i = 0; i < 6; ++i) Polygon_render(&c->polys[i]);
    glEnd();

    glPopMatrix();
}

