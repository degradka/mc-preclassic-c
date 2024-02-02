#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "../phys/aabb.h"
#include <GL/gl.h>
#include <stdlib.h>

typedef struct {
    float m_Frustum[6][4];
} Frustum;

Frustum* Frustum_create();
void Frustum_destroy(Frustum* frustum);
void Frustum_calculate(Frustum* frustum);
int Frustum_pointInFrustum(const Frustum* frustum, float x, float y, float z);
int Frustum_sphereInFrustum(const Frustum* frustum, float x, float y, float z, float radius);
int Frustum_cubeInFrustum(Frustum* frustum, float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
int Frustum_aabbInFrustum(Frustum* frustum, AABB* aabb);

#endif // FRUSTUM_H