// character/zombie.c — simple human model entity

#include "zombie.h"
#include "../textures.h"
#include "../timer.h"
#include "../common.h"

#include <GL/glew.h>
#include <math.h>

static GLuint gCharTex = 0;

// 1 px = 1/16 block
#define PX (1.0f/16.0f)
#define DEG2RAD(d) ((d) * (float)M_PI / 180.0f)

// push 1 face (quad) with (x,y,z) and (u,v) already set
static inline void V(float x, float y, float z, float u, float v) {
    glTexCoord2f(u, v);
    glVertex3f(x, y, z);
}

/*
 Classic 64x32 layout (left-handed UVs, origin at top-left):

 HEAD (8x8x8), tex:
  top:    ( 8, 0) -> (16, 8)
  bottom: (16, 0) -> (24, 8)
  left:   ( 0, 8) -> ( 8,16)
  front:  ( 8, 8) -> (16,16)
  right:  (16, 8) -> (24,16)
  back:   (24, 8) -> (32,16)

 BODY (8x12x4), tex:
  top:    (20,16)->(28,20)
  bottom: (28,16)->(36,20)
  left:   (16,20)->(20,32)
  front:  (20,20)->(28,32)
  right:  (28,20)->(36,32)
  back:   (36,20)->(44,32)

 ARM (4x12x4), tex (use same for both; mirror by position):
  top:    (44,16)->(48,20)
  bottom: (48,16)->(52,20)
  left:   (40,20)->(44,32)
  front:  (44,20)->(48,32)
  right:  (48,20)->(52,32)
  back:   (52,20)->(56,32)

 LEG (4x12x4), tex:
  top:    ( 4,16)->( 8,20)
  bottom: ( 8,16)->(12,20)
  left:   ( 0,20)->( 4,32)
  front:  ( 4,20)->( 8,32)
  right:  ( 8,20)->(12,32)
  back:   (12,20)->(16,32)
*/

// convert pixel UV (0..64, 0..32) to normalized
static inline float U(int px) { return px / 64.0f; }
static inline float Vt(int py){ return py / 32.0f; }

// draws a textured box given size (w,h,d) and a UV layout (sx,sy per face)
// box is centered around origin in X/Z, with Y up from 0 to h
// faces order: +Y (top), -Y (bottom), -Z (front), +Z (back), -X (left), +X (right)
static void drawBox(float w, float h, float d,
                    // top
                    int t_u0,int t_v0,int t_u1,int t_v1,
                    // bottom
                    int b_u0,int b_v0,int b_u1,int b_v1,
                    // front (-Z)
                    int f_u0,int f_v0,int f_u1,int f_v1,
                    // back (+Z)
                    int bk_u0,int bk_v0,int bk_u1,int bk_v1,
                    // left (-X)
                    int l_u0,int l_v0,int l_u1,int l_v1,
                    // right (+X)
                    int r_u0,int r_v0,int r_u1,int r_v1)
{
    const float x0 = -w*0.5f, x1 =  w*0.5f;
    const float z0 = -d*0.5f, z1 =  d*0.5f;
    const float y0 = 0.0f,    y1 =  h;

    glBegin(GL_QUADS);

    // +Y (top)
    V(x0,y1,z1, U(t_u0), Vt(t_v0)); V(x1,y1,z1, U(t_u1), Vt(t_v0));
    V(x1,y1,z0, U(t_u1), Vt(t_v1)); V(x0,y1,z0, U(t_u0), Vt(t_v1));

    // -Y (bottom)
    V(x0,y0,z0, U(b_u0), Vt(b_v0)); V(x1,y0,z0, U(b_u1), Vt(b_v0));
    V(x1,y0,z1, U(b_u1), Vt(b_v1)); V(x0,y0,z1, U(b_u0), Vt(b_v1));

    // -Z (front)
    V(x0,y1,z0, U(f_u0), Vt(f_v0)); V(x1,y1,z0, U(f_u1), Vt(f_v0));
    V(x1,y0,z0, U(f_u1), Vt(f_v1)); V(x0,y0,z0, U(f_u0), Vt(f_v1));

    // +Z (back)
    V(x1,y1,z1, U(bk_u0), Vt(bk_v0)); V(x0,y1,z1, U(bk_u1), Vt(bk_v0));
    V(x0,y0,z1, U(bk_u1), Vt(bk_v1)); V(x1,y0,z1, U(bk_u0), Vt(bk_v1));

    // -X (left)
    V(x0,y1,z1, U(l_u0), Vt(l_v0)); V(x0,y1,z0, U(l_u1), Vt(l_v0));
    V(x0,y0,z0, U(l_u1), Vt(l_v1)); V(x0,y0,z1, U(l_u0), Vt(l_v1));

    // +X (right)
    V(x1,y1,z0, U(r_u0), Vt(r_v0)); V(x1,y1,z1, U(r_u1), Vt(r_v0));
    V(x1,y0,z1, U(r_u1), Vt(r_v1)); V(x1,y0,z0, U(r_u0), Vt(r_v1));

    glEnd();
}

void Zombie_loadTexture(void) {
    if (!gCharTex) gCharTex = loadTexture("resources/char.png", GL_NEAREST);
}

void Zombie_init(Zombie* z, float x, float y, float zpos) {
    z->x = x; z->y = y; z->z = zpos;
    z->yaw = 0.0f;
}

static void drawHead(void) {
    // 8x8x8 px => 0.5×0.5×0.5 blocks
    const float w = 8*PX, h = 8*PX, d = 8*PX;
    drawBox(w,h,d,
        // top, bottom
        8,0, 16,8,   16,0, 24,8,
        // front(-Z), back(+Z)
        8,8, 16,16,  24,8, 32,16,
        // left(-X),  right(+X)
        0,8,  8,16,  16,8, 24,16
    );
}

static void drawBody(void) {
    // 8x12x4 px => 0.5×0.75×0.25 blocks
    const float w = 8*PX, h = 12*PX, d = 4*PX;
    drawBox(w,h,d,
        20,16, 28,20,   28,16, 36,20,
        20,20, 28,32,   36,20, 44,32,
        16,20, 20,32,   28,20, 36,32
    );
}

static void drawArm(void) {
    // 4x12x4 px => 0.25×0.75×0.25 blocks
    const float w = 4*PX, h = 12*PX, d = 4*PX;
    drawBox(w,h,d,
        44,16, 48,20,   48,16, 52,20,
        44,20, 48,32,   52,20, 56,32,
        40,20, 44,32,   48,20, 52,32
    );
}

static void drawLeg(void) {
    // 4x12x4 px => 0.25×0.75×0.25 blocks
    const float w = 4*PX, h = 12*PX, d = 4*PX;
    drawBox(w,h,d,
        4,16, 8,20,   8,16, 12,20,
        4,20, 8,32,   12,20, 16,32,
        0,20, 4,32,   8,20, 12,32
    );
}

void Zombie_render(const Zombie* z, float partialTicks) {
    (void)partialTicks;
    if (!gCharTex) return;

    glPushMatrix();
    glTranslatef(z->x, z->y, z->z);
    glRotatef(z->yaw, 0.f, 1.f, 0.f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gCharTex);

    // BODY reference frame: feet at y=0, top of head at y=2.0 blocks
    // layout (centered on X/Z):
    // legs: at y=0..0.75, spaced by 0.25
    // body: at y=0.75..1.5
    // head: atop body at y=1.5..2.0

    // draw legs
    glPushMatrix(); glTranslatef(-0.125f, 0.0f, 0.0f); drawLeg(); glPopMatrix(); // left
    glPushMatrix(); glTranslatef(+0.125f, 0.0f, 0.0f); drawLeg(); glPopMatrix(); // right

    // draw body
    glPushMatrix(); glTranslatef(0.0f, 12*PX, 0.0f); drawBody(); glPopMatrix();

    // draw arms (shoulder at body top)
    glPushMatrix(); glTranslatef(-(8*PX)/2 - (4*PX)/2, 12*PX, 0.0f); drawArm(); glPopMatrix(); // left
    glPushMatrix(); glTranslatef( +(8*PX)/2 + (4*PX)/2, 12*PX, 0.0f); drawArm(); glPopMatrix(); // right

    // head
    glPushMatrix(); glTranslatef(0.0f, 12*PX + 8*PX, 0.0f); drawHead(); glPopMatrix();

    glPopMatrix();
}

void Zombie_tick(Zombie* z) {
    const float T = 4000.0f; // ms
    float t = (float)(currentTimeMillis() % (long long)T) / T;
    z->yaw = sinf(t * 2.0f * (float)M_PI) * 15.0f;
}