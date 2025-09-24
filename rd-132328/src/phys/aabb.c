// phys/aabb.c — axis-aligned bounding boxes & collision helpers

#include "aabb.h"

AABB AABB_create(double minX, double minY, double minZ, double maxX, double maxY, double maxZ) {
    AABB aabb;
    aabb.minX = minX; aabb.minY = minY; aabb.minZ = minZ;
    aabb.maxX = maxX; aabb.maxY = maxY; aabb.maxZ = maxZ;
    return aabb;
}

AABB AABB_clone(const AABB* a) {
    return AABB_create(a->minX, a->minY, a->minZ, a->maxX, a->maxY, a->maxZ);
}

AABB AABB_expand(const AABB* a, double x, double y, double z) {
    double minX = a->minX, minY = a->minY, minZ = a->minZ;
    double maxX = a->maxX, maxY = a->maxY, maxZ = a->maxZ;

    if (x < 0.0) minX += x; else maxX += x;
    if (y < 0.0) minY += y; else maxY += y;
    if (z < 0.0) minZ += z; else maxZ += z;

    return AABB_create(minX, minY, minZ, maxX, maxY, maxZ);
}

AABB AABB_grow(const AABB* a, double x, double y, double z) {
    return AABB_create(a->minX - x, a->minY - y, a->minZ - z,
                       a->maxX + x, a->maxY + y, a->maxZ + z);
}

double AABB_clipXCollide(const AABB* a, const AABB* b, double x) {
    if (b->maxY <= a->minY || b->minY >= a->maxY) return x;
    if (b->maxZ <= a->minZ || b->minZ >= a->maxZ) return x;

    if (x > 0.0 && b->maxX <= a->minX) {
        double d = a->minX - b->maxX;
        return (d < x) ? d : x;
    }
    if (x < 0.0 && b->minX >= a->maxX) {
        double d = a->maxX - b->minX;
        return (d > x) ? d : x;
    }
    return x;
}

double AABB_clipYCollide(const AABB* a, const AABB* b, double y) {
    if (b->maxX <= a->minX || b->minX >= a->maxX) return y;
    if (b->maxZ <= a->minZ || b->minZ >= a->maxZ) return y;

    if (y > 0.0 && b->maxY <= a->minY) {
        double d = a->minY - b->maxY;
        return (d < y) ? d : y;
    }
    if (y < 0.0 && b->minY >= a->maxY) {
        double d = a->maxY - b->minY;
        return (d > y) ? d : y;
    }
    return y;
}

double AABB_clipZCollide(const AABB* a, const AABB* b, double z) {
    if (b->maxX <= a->minX || b->minX >= a->maxX) return z;
    if (b->maxY <= a->minY || b->minY >= a->maxY) return z;

    if (z > 0.0 && b->maxZ <= a->minZ) {
        double d = a->minZ - b->maxZ;
        return (d < z) ? d : z;
    }
    if (z < 0.0 && b->minZ >= a->maxZ) {
        double d = a->maxZ - b->minZ;
        return (d > z) ? d : z;
    }
    return z;
}

int AABB_intersects(const AABB* a, const AABB* b) {
    if (b->maxX <= a->minX || b->minX >= a->maxX) return 0;
    if (b->maxY <= a->minY || b->minY >= a->maxY) return 0;
    return (b->maxZ > a->minZ) && (b->minZ < a->maxZ);
}

void AABB_move(AABB* a, double x, double y, double z) {
    a->minX += x; a->minY += y; a->minZ += z;
    a->maxX += x; a->maxY += y; a->maxZ += z;
}

AABB AABB_offset(const AABB* a, double x, double y, double z) {
    return AABB_create(a->minX + x, a->minY + y, a->minZ + z,
                       a->maxX + x, a->maxY + y, a->maxZ + z);
}
