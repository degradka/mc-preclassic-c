// aabb.c

#include "aabb.h"

AABB AABB_create(double minX, double minY, double minZ, double maxX, double maxY, double maxZ) {
    AABB aabb;
    aabb.minX = minX;
    aabb.minY = minY;
    aabb.minZ = minZ;
    aabb.maxX = maxX;
    aabb.maxY = maxY;
    aabb.maxZ = maxZ;
    return aabb;
}

AABB AABB_clone(const AABB* aabb) {
    return AABB_create(aabb->minX, aabb->minY, aabb->minZ, aabb->maxX, aabb->maxY, aabb->maxZ);
}

AABB AABB_expand(const AABB* aabb, double x, double y, double z) {
    double minX = aabb->minX;
    double minY = aabb->minY;
    double minZ = aabb->minZ;
    double maxX = aabb->maxX;
    double maxY = aabb->maxY;
    double maxZ = aabb->maxZ;

    // Handle expanding of min/max x
    if (x < 0.0) {
        minX += x;
    } else {
        maxX += x;
    }

    // Handle expanding of min/max y
    if (y < 0.0) {
        minY += y;
    } else {
        maxY += y;
    }

    // Handle expanding of min/max z
    if (z < 0.0) {
        minZ += z;
    } else {
        maxZ += z;
    }

    // Create new bounding box
    return AABB_create(minX, minY, minZ, maxX, maxY, maxZ);
}

AABB AABB_grow(const AABB* aabb, double x, double y, double z) {
    return AABB_create(aabb->minX - x, aabb->minY - y, aabb->minZ - z, aabb->maxX + x, aabb->maxY + y, aabb->maxZ + z);
}

double AABB_clipXCollide(const AABB* aabb, const AABB* other, double x) {
    // Check if the boxes are colliding on the Y axis
    if (other->maxY <= aabb->minY || other->minY >= aabb->maxY) {
        return x;
    }

    // Check if the boxes are colliding on the Z axis
    if (other->maxZ <= aabb->minZ || other->minZ >= aabb->maxZ) {
        return x;
    }

    // Check for collision if the X axis of the current box is bigger
    if (x > 0.0 && other->maxX <= aabb->minX) {
        double max = aabb->minX - other->maxX;
        return (max < x) ? max : x;
    }

    // Check for collision if the X axis of the current box is smaller
    if (x < 0.0 && other->minX >= aabb->maxX) {
        double max = aabb->maxX - other->minX;
        return (max > x) ? max : x;
    }

    return x;
}

double AABB_clipYCollide(const AABB* aabb, const AABB* other, double y) {
    // Check if the boxes are colliding on the X axis
    if (other->maxX <= aabb->minX || other->minX >= aabb->maxX) {
        return y;
    }

    // Check if the boxes are colliding on the Z axis
    if (other->maxZ <= aabb->minZ || other->minZ >= aabb->maxZ) {
        return y;
    }

    // Check for collision if the Y axis of the current box is bigger
    if (y > 0.0 && other->maxY <= aabb->minY) {
        double max = aabb->minY - other->maxY;
        return (max < y) ? max : y;
    }

    // Check for collision if the Y axis of the current box is smaller
    if (y < 0.0 && other->minY >= aabb->maxY) {
        double max = aabb->maxY - other->minY;
        return (max > y) ? max : y;
    }

    return y;
}

double AABB_clipZCollide(const AABB* aabb, const AABB* other, double z) {
    // Check if the boxes are colliding on the X axis
    if (other->maxX <= aabb->minX || other->minX >= aabb->maxX) {
        return z;
    }

    // Check if the boxes are colliding on the Y axis
    if (other->maxY <= aabb->minY || other->minY >= aabb->maxY) {
        return z;
    }

    // Check for collision if the Z axis of the current box is bigger
    if (z > 0.0 && other->maxZ <= aabb->minZ) {
        double max = aabb->minZ - other->maxZ;
        return (max < z) ? max : z;
    }

    // Check for collision if the Z axis of the current box is smaller
    if (z < 0.0 && other->minZ >= aabb->maxZ) {
        double max = aabb->maxZ - other->minZ;
        return (max > z) ? max : z;
    }

    return z;
}

int AABB_intersects(const AABB* aabb, const AABB* other) {
    // Check on X axis
    if (other->maxX <= aabb->minX || other->minX >= aabb->maxX) {
        return 0;
    }

    // Check on Y axis
    if (other->maxY <= aabb->minY || other->minY >= aabb->maxY) {
        return 0;
    }

    // Check on Z axis
    return (other->maxZ > aabb->minZ) && (other->minZ < aabb->maxZ);
}

void AABB_move(AABB* aabb, double x, double y, double z) {
    aabb->minX += x;
    aabb->minY += y;
    aabb->minZ += z;
    aabb->maxX += x;
    aabb->maxY += y;
    aabb->maxZ += z;
}

AABB AABB_offset(const AABB* aabb, double x, double y, double z) {
    return AABB_create(aabb->minX + x, aabb->minY + y, aabb->minZ + z, aabb->maxX + x, aabb->maxY + y, aabb->maxZ + z);
}
