// phys/aabb.h — axis-aligned bounding boxes & collision helpers

#ifndef AABB_H
#define AABB_H

typedef struct {
    double minX, minY, minZ;
    double maxX, maxY, maxZ;
} AABB;

AABB   AABB_create(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
AABB   AABB_clone(const AABB* a);
AABB   AABB_expand(const AABB* a, double x, double y, double z);
AABB   AABB_grow(const AABB* a, double x, double y, double z);
double AABB_clipXCollide(const AABB* a, const AABB* b, double x);
double AABB_clipYCollide(const AABB* a, const AABB* b, double y);
double AABB_clipZCollide(const AABB* a, const AABB* b, double z);
int    AABB_intersects(const AABB* a, const AABB* b);
void   AABB_move(AABB* a, double x, double y, double z);
AABB   AABB_offset(const AABB* a, double x, double y, double z);

#endif // AABB_H