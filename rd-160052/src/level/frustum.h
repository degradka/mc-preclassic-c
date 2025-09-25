// level/frustum.h — extract frustum planes from GL matrices & AABB test

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "../phys/aabb.h"

typedef struct Frustum {
    float m_Frustum[6][4];
    float proj[16];
    float modl[16];
    float clip[16];
} Frustum;

extern Frustum frustum;

int  frustum_isVisible(const AABB* aabb);
void frustum_calculate(void);

#endif // FRUSTUM_H