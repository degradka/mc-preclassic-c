// entity.h — parent entity with physics and movement

#ifndef ENTITY_H
#define ENTITY_H

#include "level/level.h"
#include "phys/aabb.h"
#include <stdbool.h>

typedef struct Entity {
    Level* level;

    double x, y, z;
    double prevX, prevY, prevZ;
    double motionX, motionY, motionZ;
    float  xRotation, yRotation;

    AABB   boundingBox;
    float boundingBoxWidth;
    float boundingBoxHeight;

    bool   onGround;
    float  heightOffset;
    bool   removed;
} Entity;

void Entity_init(Entity* e, Level* level);
void Entity_setPosition(Entity* e, double x, double y, double z);
void Entity_resetPosition(Entity* e);
void Entity_turn(Entity* e, float dx, float dy);
void Entity_onTick(Entity* e);
void Entity_move(Entity* e, double dx, double dy, double dz);
void Entity_moveRelative(Entity* e, float x, float z, float speed);
bool Entity_isLit(const Entity* e);
void Entity_remove(Entity* e);
void Entity_setSize(Entity* e, float width, float height);

#endif

