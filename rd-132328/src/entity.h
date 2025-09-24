// entity.h — base physics: movement, camera turn, collision

#ifndef ENTITY_H
#define ENTITY_H

#include "common.h"
#include "level/level.h"
#include "phys/aabb.h"
#include <math.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

typedef struct Entity {
    Level* level;
    float  x, y, z;
    double prevX, prevY, prevZ;
    double motionX, motionY, motionZ;
    float  yRotation, xRotation;
    bool   onGround;
    AABB   boundingBox;
} Entity;

void Entity_init(Entity* e, Level* level);
void Entity_setPosition(Entity* e, float x, float y, float z);
void Entity_resetPosition(Entity* e);
void Entity_turn(Entity* e, GLFWwindow* window, float dx, float dy);
void Entity_move(Entity* e, double dx, double dy, double dz);
void Entity_tick(Entity* e, GLFWwindow* window);
void Entity_moveRelative(Entity* e, float x, float z, float speed);

#endif  // ENTITY_H
