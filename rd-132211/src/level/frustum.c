#include "frustum.h"

#include <math.h>

#define RIGHT 0
#define LEFT 1
#define BOTTOM 2
#define TOP 3
#define BACK 4
#define FRONT 5

#define A 0
#define B 1
#define C 2
#define D 3

Frustum* Frustum_create() {
    Frustum* frustum = (Frustum*)malloc(sizeof(Frustum));
    return frustum;
}

void Frustum_destroy(Frustum* frustum) {
    free(frustum);
}

static void normalizePlane(float frustum[6][4], int side) {
    float magnitude = sqrt(frustum[side][A] * frustum[side][A] +
                           frustum[side][B] * frustum[side][B] +
                           frustum[side][C] * frustum[side][C]);

    frustum[side][A] /= magnitude;
    frustum[side][B] /= magnitude;
    frustum[side][C] /= magnitude;
    frustum[side][D] /= magnitude;
}

void Frustum_calculate(Frustum* frustum) {
    float proj[16];  // This will hold our projection matrix
    float modl[16];  // This will hold our modelview matrix
    float clip[16];  // This will hold the clipping planes

    // glGetFloat() is used to extract information about our OpenGL world.
    // Below, we pass in GL_PROJECTION_MATRIX to abstract our projection matrix.
    // It then stores the matrix into an array of [16].
    glGetFloatv(GL_PROJECTION_MATRIX, proj);

    // By passing in GL_MODELVIEW_MATRIX, we can abstract our model view matrix.
    // This also stores it in an array of [16].
    glGetFloatv(GL_MODELVIEW_MATRIX, modl);

    // Now that we have our modelview and projection matrix, if we combine these 2 matrices,
    // it will give us our clipping planes.  To combine 2 matrices, we multiply them.

    clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
    clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
    clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
    clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

    clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
    clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
    clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
    clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

    clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
    clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
    clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

    clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

    // Now we actually want to get the sides of the frustum. To do this, we take
    // the clipping planes we received above and extract the sides from them.

    // This will extract the RIGHT side of the frustum
    frustum->m_Frustum[RIGHT][A] = clip[3] - clip[0];
    frustum->m_Frustum[RIGHT][B] = clip[7] - clip[4];
    frustum->m_Frustum[RIGHT][C] = clip[11] - clip[8];
    frustum->m_Frustum[RIGHT][D] = clip[15] - clip[12];

    // Normalize the RIGHT side
    normalizePlane(frustum->m_Frustum, RIGHT);

    // This will extract the LEFT side of the frustum
    frustum->m_Frustum[LEFT][A] = clip[3] + clip[0];
    frustum->m_Frustum[LEFT][B] = clip[7] + clip[4];
    frustum->m_Frustum[LEFT][C] = clip[11] + clip[8];
    frustum->m_Frustum[LEFT][D] = clip[15] + clip[12];

    // Normalize the LEFT side
    normalizePlane(frustum->m_Frustum, LEFT);

    // This will extract the BOTTOM side of the frustum
    frustum->m_Frustum[BOTTOM][A] = clip[3] + clip[1];
    frustum->m_Frustum[BOTTOM][B] = clip[7] + clip[5];
    frustum->m_Frustum[BOTTOM][C] = clip[11] + clip[9];
    frustum->m_Frustum[BOTTOM][D] = clip[15] + clip[13];

    // Normalize the BOTTOM side
    normalizePlane(frustum->m_Frustum, BOTTOM);

    // This will extract the TOP side of the frustum
    frustum->m_Frustum[TOP][A] = clip[3] - clip[1];
    frustum->m_Frustum[TOP][B] = clip[7] - clip[5];
    frustum->m_Frustum[TOP][C] = clip[11] - clip[9];
    frustum->m_Frustum[TOP][D] = clip[15] - clip[13];

    // Normalize the TOP side
    normalizePlane(frustum->m_Frustum, TOP);

    // This will extract the BACK side of the frustum
    frustum->m_Frustum[BACK][A] = clip[3] - clip[2];
    frustum->m_Frustum[BACK][B] = clip[7] - clip[6];
    frustum->m_Frustum[BACK][C] = clip[11] - clip[10];
    frustum->m_Frustum[BACK][D] = clip[15] - clip[14];

    // Normalize the BACK side
    normalizePlane(frustum->m_Frustum, BACK);

    // This will extract the FRONT side of the frustum
    frustum->m_Frustum[FRONT][A] = clip[3] + clip[2];
    frustum->m_Frustum[FRONT][B] = clip[7] + clip[6];
    frustum->m_Frustum[FRONT][C] = clip[11] + clip[10];
    frustum->m_Frustum[FRONT][D] = clip[15] + clip[14];

    // Normalize the FRONT side
    normalizePlane(frustum->m_Frustum, FRONT);
}

int Frustum_pointInFrustum(const Frustum* frustum, float x, float y, float z) {
    for (int i = 0; i < 6; i++) {
        if (frustum->m_Frustum[i][0] * x + frustum->m_Frustum[i][1] * y +
            frustum->m_Frustum[i][2] * z + frustum->m_Frustum[i][3] <= 0) {
            return 0; // Point is behind a side of the frustum
        }
    }
    return 1; // Point is inside the frustum
}

int Frustum_sphereInFrustum(const Frustum* frustum, float x, float y, float z, float radius) {
    for (int i = 0; i < 6; i++) {
        if (frustum->m_Frustum[i][0] * x + frustum->m_Frustum[i][1] * y +
            frustum->m_Frustum[i][2] * z + frustum->m_Frustum[i][3] <= -radius) {
            return 0; // Sphere is outside of the frustum
        }
    }
    return 1; // Sphere is inside or intersecting the frustum
}

int Frustum_cubeInFrustum(Frustum* frustum, float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
    for (int i = 0; i < 6; i++) {
        if (frustum->m_Frustum[i][A] * minX + frustum->m_Frustum[i][B] * minY +
                frustum->m_Frustum[i][C] * minZ + frustum->m_Frustum[i][D] > 0)
            continue;
        if (frustum->m_Frustum[i][A] * maxX + frustum->m_Frustum[i][B] * minY +
                frustum->m_Frustum[i][C] * minZ + frustum->m_Frustum[i][D] > 0)
            continue;
        if (frustum->m_Frustum[i][A] * minX + frustum->m_Frustum[i][B] * maxY +
                frustum->m_Frustum[i][C] * minZ + frustum->m_Frustum[i][D] > 0)
            continue;
        if (frustum->m_Frustum[i][A] * maxX + frustum->m_Frustum[i][B] * maxY +
                frustum->m_Frustum[i][C] * minZ + frustum->m_Frustum[i][D] > 0)
            continue;
        if (frustum->m_Frustum[i][A] * minX + frustum->m_Frustum[i][B] * minY +
                frustum->m_Frustum[i][C] * maxZ + frustum->m_Frustum[i][D] > 0)
            continue;
        if (frustum->m_Frustum[i][A] * maxX + frustum->m_Frustum[i][B] * minY +
                frustum->m_Frustum[i][C] * maxZ + frustum->m_Frustum[i][D] > 0)
            continue;
        if (frustum->m_Frustum[i][A] * minX + frustum->m_Frustum[i][B] * maxY +
                frustum->m_Frustum[i][C] * maxZ + frustum->m_Frustum[i][D] > 0)
            continue;
        if (frustum->m_Frustum[i][A] * maxX + frustum->m_Frustum[i][B] * maxY +
                frustum->m_Frustum[i][C] * maxZ + frustum->m_Frustum[i][D] > 0)
            continue;

        // If we get here, it isn't in the frustum
        return 0;
    }

    return 1;
}

int Frustum_aabbInFrustum(Frustum* frustum, AABB* aabb) {
    return Frustum_cubeInFrustum(frustum, (float)aabb->minX, (float)aabb->minY, (float)aabb->minZ,
                                   (float)aabb->maxX, (float)aabb->maxY, (float)aabb->maxZ);
}