// character/zombie.h

#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "../entity.h"
#include "cube.h"

typedef struct {
    Entity base;
    Cube   head, body, rightArm, leftArm, rightLeg, leftLeg;
    double rotation;
    double rotationMotionFactor;
    float  timeOffset;
    float  speed;
} Zombie;

void Zombie_init(Zombie* z, Level* level, double x, double y, double zpos);
void Zombie_onTick(Zombie* z);
void Zombie_render(const Zombie* z, float partialTicks);

#endif
