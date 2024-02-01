// aabb.h

#ifndef AABB_H
#define AABB_H

typedef struct {
    double minX;
    double minY;
    double minZ;
    double maxX;
    double maxY;
    double maxZ;
} AABB;

AABB AABB_create(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
AABB AABB_clone(const AABB* aabb);
AABB AABB_expand(const AABB* aabb, double x, double y, double z);
AABB AABB_grow(const AABB* aabb, double x, double y, double z);
double AABB_clipXCollide(const AABB* aabb, const AABB* other, double x);
double AABB_clipYCollide(const AABB* aabb, const AABB* other, double y);
double AABB_clipZCollide(const AABB* aabb, const AABB* other, double z);
int AABB_intersects(const AABB* aabb, const AABB* other);
void AABB_move(AABB* aabb, double x, double y, double z);
AABB AABB_offset(const AABB* aabb, double x, double y, double z);

#endif // AABB_H