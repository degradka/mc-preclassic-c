// character/polygon.c

#include "polygon.h"
#include <GL/glew.h>

void Polygon_init_uv(Polygon* p, Vertex a, Vertex b, Vertex c, Vertex d,
                     int minU, int minV, int maxU, int maxV) {
    p->v[0] = Vertex_remap(a, maxU, minV);
    p->v[1] = Vertex_remap(b, minU, minV);
    p->v[2] = Vertex_remap(c, minU, maxV);
    p->v[3] = Vertex_remap(d, maxU, maxV);
}

void Polygon_render(const Polygon* p) {
    glColor3f(1.f, 1.f, 1.f);
    glTexCoord2f(p->v[3].u / 64.f, p->v[3].v / 32.f); glVertex3f(p->v[3].pos.x, p->v[3].pos.y, p->v[3].pos.z);
    glTexCoord2f(p->v[2].u / 64.f, p->v[2].v / 32.f); glVertex3f(p->v[2].pos.x, p->v[2].pos.y, p->v[2].pos.z);
    glTexCoord2f(p->v[1].u / 64.f, p->v[1].v / 32.f); glVertex3f(p->v[1].pos.x, p->v[1].pos.y, p->v[1].pos.z);
    glTexCoord2f(p->v[0].u / 64.f, p->v[0].v / 32.f); glVertex3f(p->v[0].pos.x, p->v[0].pos.y, p->v[0].pos.z);
}
