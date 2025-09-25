// character/vec3.h

#ifndef VEC3_H
#define VEC3_H

typedef struct { float x, y, z; } Vec3;
static inline Vec3 Vec3_make(float x, float y, float z) { Vec3 v = { x, y, z }; return v; }
static inline Vec3 Vec3_lerp(Vec3 a, Vec3 b, float t) {
    return (Vec3){ a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t };
}

#endif
