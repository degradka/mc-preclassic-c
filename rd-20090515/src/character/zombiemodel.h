// character/zombiemodel.h — extracted zombie model (cubes + animation)

#ifndef ZOMBIE_MODEL_H
#define ZOMBIE_MODEL_H

#include "cube.h"

typedef struct {
    Cube head;
    Cube body;
    Cube rightArm, leftArm;
    Cube rightLeg, leftLeg;
} ZombieModel;

void ZombieModel_init  (ZombieModel* m);
void ZombieModel_render(const ZombieModel* m, double time);

#endif