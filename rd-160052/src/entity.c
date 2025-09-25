// entity.c — parent entity with physics and movement

#include "entity.h"
#include <math.h>
#include <stdlib.h>

#define DEG2RAD(d) ((d) * M_PI / 180.0)

void Entity_init(Entity* e, Level* level) {
    e->level = level;
    e->xRotation = e->yRotation = 0.f;
    e->motionX = e->motionY = e->motionZ = 0.0;
    e->onGround = false;
    e->heightOffset = 0.0f;
    e->removed = false;
    Entity_resetPosition(e);
}

void Entity_setPosition(Entity* e, double x, double y, double z) {
    e->x = x; e->y = y; e->z = z;
    const float w = 0.3f, h = 0.9f;
    e->boundingBox = AABB_create((float)x - w, (float)y - h, (float)z - w,
                                 (float)x + w, (float)y + h, (float)z + w);
    e->prevX = x; e->prevY = y; e->prevZ = z;
}

void Entity_resetPosition(Entity* e) {
    const float x = (float)rand() / (float)RAND_MAX * e->level->width;
    const float y = (float)e->level->depth + 3.0f;
    const float z = (float)rand() / (float)RAND_MAX * e->level->height;
    Entity_setPosition(e, x, y, z);
}

void Entity_turn(Entity* e, float dx, float dy) {
    e->yRotation += dx * 0.15f;
    e->xRotation -= dy * 0.15f;
    if (e->xRotation >  90.0f) e->xRotation =  90.0f;
    if (e->xRotation < -90.0f) e->xRotation = -90.0f;
}

void Entity_tick(Entity* e) {
    e->prevX = e->x;
    e->prevY = e->y;
    e->prevZ = e->z;
}

void Entity_move(Entity* e, double dx, double dy, double dz) {
    const double ox = dx, oy = dy, oz = dz;

    AABB expanded = AABB_expand(&e->boundingBox, dx, dy, dz);
    ArrayList_AABB hits = Level_getCubes(e->level, &expanded);

    for (int i = 0; i < hits.size; ++i) dy = AABB_clipYCollide(&hits.aabbs[i], &e->boundingBox, dy);
    AABB_move(&e->boundingBox, 0.0, dy, 0.0);

    for (int i = 0; i < hits.size; ++i) dx = AABB_clipXCollide(&hits.aabbs[i], &e->boundingBox, dx);
    AABB_move(&e->boundingBox, dx, 0.0, 0.0);

    for (int i = 0; i < hits.size; ++i) dz = AABB_clipZCollide(&hits.aabbs[i], &e->boundingBox, dz);
    AABB_move(&e->boundingBox, 0.0, 0.0, dz);

    e->onGround = (oy != dy) && (oy < 0.0);

    if (ox != dx) e->motionX = 0.0;
    if (oy != dy) e->motionY = 0.0;
    if (oz != dz) e->motionZ = 0.0;

    e->x = (e->boundingBox.minX + e->boundingBox.maxX) * 0.5;
    e->y =  e->boundingBox.minY + e->heightOffset;
    e->z = (e->boundingBox.minZ + e->boundingBox.maxZ) * 0.5;

    if (hits.aabbs) free(hits.aabbs);
}

void Entity_moveRelative(Entity* e, float x, float z, float speed) {
    float d2 = x*x + z*z;
    if (d2 < 0.01f) return;

    float k = speed / sqrtf(d2);
    x *= k; z *= k;

    const double s = sin(DEG2RAD(e->yRotation));
    const double c = cos(DEG2RAD(e->yRotation));

    e->motionX += x * c - z * s;
    e->motionZ += z * c + x * s;
}

bool Entity_isLit(const Entity* e) {
    return Level_isLit(e->level, (int)e->x, (int)e->y, (int)e->z);
}

void Entity_remove(Entity* e) {
    e->removed = true;
}