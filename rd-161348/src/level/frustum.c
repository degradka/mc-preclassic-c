// level/frustum.c — extract frustum planes from GL matrices & AABB test

#include "frustum.h"
#include <math.h>
#include <GL/glew.h>

Frustum frustum;

static void frustum_normalizePlane(float f[6][4], int side) {
    float m = (float)sqrt((double)(f[side][0]*f[side][0] + f[side][1]*f[side][1] + f[side][2]*f[side][2]));
    f[side][0] /= m; f[side][1] /= m; f[side][2] /= m; f[side][3] /= m;
}

void frustum_calculate(void) {
    glGetFloatv(GL_PROJECTION_MATRIX, frustum.proj);
    glGetFloatv(GL_MODELVIEW_MATRIX,  frustum.modl);

    float* p = frustum.proj;
    float* m = frustum.modl;
    float* c = frustum.clip;

    c[0]  = m[0]*p[0]  + m[1]*p[4]  + m[2]*p[8]  + m[3]*p[12];
    c[1]  = m[0]*p[1]  + m[1]*p[5]  + m[2]*p[9]  + m[3]*p[13];
    c[2]  = m[0]*p[2]  + m[1]*p[6]  + m[2]*p[10] + m[3]*p[14];
    c[3]  = m[0]*p[3]  + m[1]*p[7]  + m[2]*p[11] + m[3]*p[15];

    c[4]  = m[4]*p[0]  + m[5]*p[4]  + m[6]*p[8]  + m[7]*p[12];
    c[5]  = m[4]*p[1]  + m[5]*p[5]  + m[6]*p[9]  + m[7]*p[13];
    c[6]  = m[4]*p[2]  + m[5]*p[6]  + m[6]*p[10] + m[7]*p[14];
    c[7]  = m[4]*p[3]  + m[5]*p[7]  + m[6]*p[11] + m[7]*p[15];

    c[8]  = m[8]*p[0]  + m[9]*p[4]  + m[10]*p[8] + m[11]*p[12];
    c[9]  = m[8]*p[1]  + m[9]*p[5]  + m[10]*p[9] + m[11]*p[13];
    c[10] = m[8]*p[2]  + m[9]*p[6]  + m[10]*p[10]+ m[11]*p[14];
    c[11] = m[8]*p[3]  + m[9]*p[7]  + m[10]*p[11]+ m[11]*p[15];

    c[12] = m[12]*p[0] + m[13]*p[4] + m[14]*p[8] + m[15]*p[12];
    c[13] = m[12]*p[1] + m[13]*p[5] + m[14]*p[9] + m[15]*p[13];
    c[14] = m[12]*p[2] + m[13]*p[6] + m[14]*p[10]+ m[15]*p[14];
    c[15] = m[12]*p[3] + m[13]*p[7] + m[14]*p[11]+ m[15]*p[15];

    // right
    frustum.m_Frustum[0][0] = c[3] - c[0];
    frustum.m_Frustum[0][1] = c[7] - c[4];
    frustum.m_Frustum[0][2] = c[11]- c[8];
    frustum.m_Frustum[0][3] = c[15]- c[12];
    frustum_normalizePlane(frustum.m_Frustum, 0);

    // left
    frustum.m_Frustum[1][0] = c[3] + c[0];
    frustum.m_Frustum[1][1] = c[7] + c[4];
    frustum.m_Frustum[1][2] = c[11]+ c[8];
    frustum.m_Frustum[1][3] = c[15]+ c[12];
    frustum_normalizePlane(frustum.m_Frustum, 1);

    // bottom
    frustum.m_Frustum[2][0] = c[3] + c[1];
    frustum.m_Frustum[2][1] = c[7] + c[5];
    frustum.m_Frustum[2][2] = c[11]+ c[9];
    frustum.m_Frustum[2][3] = c[15]+ c[13];
    frustum_normalizePlane(frustum.m_Frustum, 2);

    // top
    frustum.m_Frustum[3][0] = c[3] - c[1];
    frustum.m_Frustum[3][1] = c[7] - c[5];
    frustum.m_Frustum[3][2] = c[11]- c[9];
    frustum.m_Frustum[3][3] = c[15]- c[13];
    frustum_normalizePlane(frustum.m_Frustum, 3);

    // back
    frustum.m_Frustum[4][0] = c[3] - c[2];
    frustum.m_Frustum[4][1] = c[7] - c[6];
    frustum.m_Frustum[4][2] = c[11]- c[10];
    frustum.m_Frustum[4][3] = c[15]- c[14];
    frustum_normalizePlane(frustum.m_Frustum, 4);

    // front
    frustum.m_Frustum[5][0] = c[3] + c[2];
    frustum.m_Frustum[5][1] = c[7] + c[6];
    frustum.m_Frustum[5][2] = c[11]+ c[10];
    frustum.m_Frustum[5][3] = c[15]+ c[14];
    frustum_normalizePlane(frustum.m_Frustum, 5);
}

static int frustum_cubeIn(float x1, float y1, float z1, float x2, float y2, float z2) {
    for (int i = 0; i < 6; ++i) {
        if (frustum.m_Frustum[i][0]*x1 + frustum.m_Frustum[i][1]*y1 + frustum.m_Frustum[i][2]*z1 + frustum.m_Frustum[i][3] > 0) continue;
        if (frustum.m_Frustum[i][0]*x2 + frustum.m_Frustum[i][1]*y1 + frustum.m_Frustum[i][2]*z1 + frustum.m_Frustum[i][3] > 0) continue;
        if (frustum.m_Frustum[i][0]*x1 + frustum.m_Frustum[i][1]*y2 + frustum.m_Frustum[i][2]*z1 + frustum.m_Frustum[i][3] > 0) continue;
        if (frustum.m_Frustum[i][0]*x2 + frustum.m_Frustum[i][1]*y2 + frustum.m_Frustum[i][2]*z1 + frustum.m_Frustum[i][3] > 0) continue;
        if (frustum.m_Frustum[i][0]*x1 + frustum.m_Frustum[i][1]*y1 + frustum.m_Frustum[i][2]*z2 + frustum.m_Frustum[i][3] > 0) continue;
        if (frustum.m_Frustum[i][0]*x2 + frustum.m_Frustum[i][1]*y1 + frustum.m_Frustum[i][2]*z2 + frustum.m_Frustum[i][3] > 0) continue;
        if (frustum.m_Frustum[i][0]*x1 + frustum.m_Frustum[i][1]*y2 + frustum.m_Frustum[i][2]*z2 + frustum.m_Frustum[i][3] > 0) continue;
        if (frustum.m_Frustum[i][0]*x2 + frustum.m_Frustum[i][1]*y2 + frustum.m_Frustum[i][2]*z2 + frustum.m_Frustum[i][3] > 0) continue;
        return 0; // all 8 corners are outside this plane
    }
    return 1;
}

int frustum_isVisible(const AABB* aabb) {
    return frustum_cubeIn((float)aabb->minX, (float)aabb->minY, (float)aabb->minZ,
                          (float)aabb->maxX, (float)aabb->maxY, (float)aabb->maxZ);
}