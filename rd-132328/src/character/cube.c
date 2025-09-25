// character/cube.c — textured box model part

#include "cube.h"
#include <string.h>

static inline void quad(Tessellator* t, float b,
                        float x0,float y0,float z0,
                        float x1,float y1,float z1,
                        float x2,float y2,float z2,
                        float x3,float y3,float z3,
                        float u0,float v0,float u1,float v1)
{
    Tessellator_color(t, b, b, b);
    Tessellator_texture(t, u0, v1); Tessellator_vertex(t, x0,y0,z0);
    Tessellator_texture(t, u0, v0); Tessellator_vertex(t, x1,y1,z1);
    Tessellator_texture(t, u1, v0); Tessellator_vertex(t, x2,y2,z2);
    Tessellator_texture(t, u1, v1); Tessellator_vertex(t, x3,y3,z3);
}

void Cube_init(Cube* c, int u, int v) {
    memset(c, 0, sizeof(*c));
    c->u = u; c->v = v;
}

void Cube_addBox(Cube* c, float x, float y, float z, int w, int h, int d) {
    c->x0 = x;       c->y0 = y;       c->z0 = z;
    c->x1 = x + w;   c->y1 = y + h;   c->z1 = z + d;
}

void Cube_setPosition(Cube* c, float px, float py, float pz) {
    c->px = px; c->py = py; c->pz = pz;
}

static inline float U(int px) { return px / 64.0f; }
static inline float V(int py) { return py / 64.0f; }

void Cube_render(const Cube* c, Tessellator* t) {
    const float x0=c->x0, y0=c->y0, z0=c->z0;
    const float x1=c->x1, y1=c->y1, z1=c->z1;

    const float ox=c->px, oy=c->py, oz=c->pz;

    const int W = (int)(x1 - x0);
    const int H = (int)(y1 - y0);
    const int D = (int)(z1 - z0);

    const int u = c->u, v = c->v;

    float u_top0 = U(u + W),       v_top0 = V(v);
    float u_top1 = U(u + W + D),   v_top1 = V(v + W);

    float u_bot0 = U(u + W + D),   v_bot0 = V(v);
    float u_bot1 = U(u + W + D + D), v_bot1 = V(v + W);

    float u_l0 = U(u),             v_l0 = V(v + W);
    float u_l1 = U(u + D),         v_l1 = V(v + W + H);

    float u_f0 = U(u + D),         v_f0 = V(v + W);
    float u_f1 = U(u + D + W),     v_f1 = V(v + W + H);

    float u_r0 = U(u + D + W),     v_r0 = V(v + W);
    float u_r1 = U(u + D + W + D), v_r1 = V(v + W + H);

    float u_b0 = U(u + D + W + D),     v_b0 = V(v + W);
    float u_b1 = U(u + D + W + D + W), v_b1 = V(v + W + H);

    const float bY = 1.0f, bX = 0.6f, bZ = 0.8f;

    // top (+Y)
    quad(t, bY,  x1+ox,y1+oy,z1+oz,  x1+ox,y1+oy,z0+oz,  x0+ox,y1+oy,z0+oz,  x0+ox,y1+oy,z1+oz,  u_top0,v_top0, u_top1,v_top1);
    // bottom (-Y)
    quad(t, bY,  x0+ox,y0+oy,z1+oz,  x0+ox,y0+oy,z0+oz,  x1+ox,y0+oy,z0+oz,  x1+ox,y0+oy,z1+oz,  u_bot0,v_bot0, u_bot1,v_bot1);
    // left (-X)
    quad(t, bX,  x0+ox,y1+oy,z1+oz,  x0+ox,y1+oy,z0+oz,  x0+ox,y0+oy,z0+oz,  x0+ox,y0+oy,z1+oz,  u_l0,v_l0, u_l1,v_l1);
    // right (+X)
    quad(t, bX,  x1+ox,y0+oy,z1+oz,  x1+ox,y0+oy,z0+oz,  x1+ox,y1+oy,z0+oz,  x1+ox,y1+oy,z1+oz,  u_r0,v_r0, u_r1,v_r1);
    // front (-Z)
    quad(t, bZ,  x0+ox,y1+oy,z0+oz,  x1+ox,y1+oy,z0+oz,  x1+ox,y0+oy,z0+oz,  x0+ox,y0+oy,z0+oz,  u_f0,v_f0, u_f1,v_f1);
    // back (+Z)
    quad(t, bZ,  x0+ox,y0+oy,z1+oz,  x1+ox,y0+oy,z1+oz,  x1+ox,y1+oy,z1+oz,  x0+ox,y1+oy,z1+oz,  u_b0,v_b0, u_b1,v_b1);
}
