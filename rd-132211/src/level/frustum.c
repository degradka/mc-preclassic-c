#include "frustum.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <GL/gl.h>

Frustum frustum;

void frustum_normalizePlane(float frustum[6][4], int side) {
    float magnitude = (float)sqrt((double)(frustum[side][0] * frustum[side][0] + frustum[side][1] * frustum[side][1] + frustum[side][2] * frustum[side][2]));
    frustum[side][0] /= magnitude;
    frustum[side][1] /= magnitude;
    frustum[side][2] /= magnitude;
    frustum[side][3] /= magnitude;
}

void frustum_calculate() {
    glGetFloatv(GL_PROJECTION_MATRIX, frustum.proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, frustum.modl);
    frustum.clip[0] = frustum.modl[0] * frustum.proj[0] + frustum.modl[1] * frustum.proj[4] + frustum.modl[2] * frustum.proj[8] + frustum.modl[3] * frustum.proj[12];
    frustum.clip[1] = frustum.modl[0] * frustum.proj[1] + frustum.modl[1] * frustum.proj[5] + frustum.modl[2] * frustum.proj[9] + frustum.modl[3] * frustum.proj[13];
    frustum.clip[2] = frustum.modl[0] * frustum.proj[2] + frustum.modl[1] * frustum.proj[6] + frustum.modl[2] * frustum.proj[10] + frustum.modl[3] * frustum.proj[14];
    frustum.clip[3] = frustum.modl[0] * frustum.proj[3] + frustum.modl[1] * frustum.proj[7] + frustum.modl[2] * frustum.proj[11] + frustum.modl[3] * frustum.proj[15];
    frustum.clip[4] = frustum.modl[4] * frustum.proj[0] + frustum.modl[5] * frustum.proj[4] + frustum.modl[6] * frustum.proj[8] + frustum.modl[7] * frustum.proj[12];
    frustum.clip[5] = frustum.modl[4] * frustum.proj[1] + frustum.modl[5] * frustum.proj[5] + frustum.modl[6] * frustum.proj[9] + frustum.modl[7] * frustum.proj[13];
    frustum.clip[6] = frustum.modl[4] * frustum.proj[2] + frustum.modl[5] * frustum.proj[6] + frustum.modl[6] * frustum.proj[10] + frustum.modl[7] * frustum.proj[14];
    frustum.clip[7] = frustum.modl[4] * frustum.proj[3] + frustum.modl[5] * frustum.proj[7] + frustum.modl[6] * frustum.proj[11] + frustum.modl[7] * frustum.proj[15];
    frustum.clip[8] = frustum.modl[8] * frustum.proj[0] + frustum.modl[9] * frustum.proj[4] + frustum.modl[10] * frustum.proj[8] + frustum.modl[11] * frustum.proj[12];
    frustum.clip[9] = frustum.modl[8] * frustum.proj[1] + frustum.modl[9] * frustum.proj[5] + frustum.modl[10] * frustum.proj[9] + frustum.modl[11] * frustum.proj[13];
    frustum.clip[10] = frustum.modl[8] * frustum.proj[2] + frustum.modl[9] * frustum.proj[6] + frustum.modl[10] * frustum.proj[10] + frustum.modl[11] * frustum.proj[14];
    frustum.clip[11] = frustum.modl[8] * frustum.proj[3] + frustum.modl[9] * frustum.proj[7] + frustum.modl[10] * frustum.proj[11] + frustum.modl[11] * frustum.proj[15];
    frustum.clip[12] = frustum.modl[12] * frustum.proj[0] + frustum.modl[13] * frustum.proj[4] + frustum.modl[14] * frustum.proj[8] + frustum.modl[15] * frustum.proj[12];
    frustum.clip[13] = frustum.modl[12] * frustum.proj[1] + frustum.modl[13] * frustum.proj[5] + frustum.modl[14] * frustum.proj[9] + frustum.modl[15] * frustum.proj[13];
    frustum.clip[14] = frustum.modl[12] * frustum.proj[2] + frustum.modl[13] * frustum.proj[6] + frustum.modl[14] * frustum.proj[10] + frustum.modl[15] * frustum.proj[14];
    frustum.clip[15] = frustum.modl[12] * frustum.proj[3] + frustum.modl[13] * frustum.proj[7] + frustum.modl[14] * frustum.proj[11] + frustum.modl[15] * frustum.proj[15];
    frustum.m_Frustum[0][0] = frustum.clip[3] - frustum.clip[0];
    frustum.m_Frustum[0][1] = frustum.clip[7] - frustum.clip[4];
    frustum.m_Frustum[0][2] = frustum.clip[11] - frustum.clip[8];
    frustum.m_Frustum[0][3] = frustum.clip[15] - frustum.clip[12];
    frustum_normalizePlane(frustum.m_Frustum, 0);
    frustum.m_Frustum[1][0] = frustum.clip[3] + frustum.clip[0];
    frustum.m_Frustum[1][1] = frustum.clip[7] + frustum.clip[4];
    frustum.m_Frustum[1][2] = frustum.clip[11] + frustum.clip[8];
    frustum.m_Frustum[1][3] = frustum.clip[15] + frustum.clip[12];
    frustum_normalizePlane(frustum.m_Frustum, 1);
    frustum.m_Frustum[2][0] = frustum.clip[3] + frustum.clip[1];
    frustum.m_Frustum[2][1] = frustum.clip[7] + frustum.clip[5];
    frustum.m_Frustum[2][2] = frustum.clip[11] + frustum.clip[9];
    frustum.m_Frustum[2][3] = frustum.clip[15] + frustum.clip[13];
    frustum_normalizePlane(frustum.m_Frustum, 2);
    frustum.m_Frustum[3][0] = frustum.clip[3] - frustum.clip[1];
    frustum.m_Frustum[3][1] = frustum.clip[7] - frustum.clip[5];
    frustum.m_Frustum[3][2] = frustum.clip[11] - frustum.clip[9];
    frustum.m_Frustum[3][3] = frustum.clip[15] - frustum.clip[13];
    frustum_normalizePlane(frustum.m_Frustum, 3);
    frustum.m_Frustum[4][0] = frustum.clip[3] - frustum.clip[2];
    frustum.m_Frustum[4][1] = frustum.clip[7] - frustum.clip[6];
    frustum.m_Frustum[4][2] = frustum.clip[11] - frustum.clip[10];
    frustum.m_Frustum[4][3] = frustum.clip[15] - frustum.clip[14];
    frustum_normalizePlane(frustum.m_Frustum, 4);
    frustum.m_Frustum[5][0] = frustum.clip[3] + frustum.clip[2];
    frustum.m_Frustum[5][1] = frustum.clip[7] + frustum.clip[6];
    frustum.m_Frustum[5][2] = frustum.clip[11] + frustum.clip[10];
    frustum.m_Frustum[5][3] = frustum.clip[15] + frustum.clip[14];
    frustum_normalizePlane(frustum.m_Frustum, 5);
}

int frustum_cubeIn(float x1, float y1, float z1, float x2, float y2, float z2) {
    for (int i = 0; i < 6; ++i) {
        if (frustum.m_Frustum[i][0] * x1 + frustum.m_Frustum[i][1] * y1 + frustum.m_Frustum[i][2] * z1 + frustum.m_Frustum[i][3] <= 0.0F && frustum.m_Frustum[i][0] * x2 + frustum.m_Frustum[i][1] * y1 + frustum.m_Frustum[i][2] * z1 + frustum.m_Frustum[i][3] <= 0.0F && frustum.m_Frustum[i][0] * x1 + frustum.m_Frustum[i][1] * y2 + frustum.m_Frustum[i][2] * z1 + frustum.m_Frustum[i][3] <= 0.0F && frustum.m_Frustum[i][0] * x2 + frustum.m_Frustum[i][1] * y2 + frustum.m_Frustum[i][2] * z1 + frustum.m_Frustum[i][3] <= 0.0F && frustum.m_Frustum[i][0] * x1 + frustum.m_Frustum[i][1] * y1 + frustum.m_Frustum[i][2] * z2 + frustum.m_Frustum[i][3] <= 0.0F && frustum.m_Frustum[i][0] * x2 + frustum.m_Frustum[i][1] * y1 + frustum.m_Frustum[i][2] * z2 + frustum.m_Frustum[i][3] <= 0.0F && frustum.m_Frustum[i][0] * x1 + frustum.m_Frustum[i][1] * y2 + frustum.m_Frustum[i][2] * z2 + frustum.m_Frustum[i][3] <= 0.0F && frustum.m_Frustum[i][0] * x2 + frustum.m_Frustum[i][1] * y2 + frustum.m_Frustum[i][2] * z2 + frustum.m_Frustum[i][3] <= 0.0F) {
            return 0;
        }
    }

    return 1;
}

int frustum_cubeInAABB(AABB* aabb) {
    return frustum_cubeIn(aabb->minX, aabb->minY, aabb->minZ, aabb->maxX, aabb->maxY, aabb->maxZ);
}
