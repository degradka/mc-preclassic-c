// character/zombiemodel.c — extracted zombie model (cubes + animation)

#include "zombiemodel.h"
#include "../common.h"
#include <math.h>

void ZombieModel_init(ZombieModel* m) {
    // head
    Cube_init(&m->head, 0, 0);
    Cube_addBox(&m->head, -4, -8, -4, 8, 8, 8);

    // body
    Cube_init(&m->body, 16, 16);
    Cube_addBox(&m->body, -4, 0, -2, 8, 12, 4);

    // right arm
    Cube_init(&m->rightArm, 40, 16);
    Cube_addBox(&m->rightArm, -3, -2, -2, 4, 12, 4);
    Cube_setPos(&m->rightArm, -5, 2, 0);

    // left arm
    Cube_init(&m->leftArm, 40, 16);
    Cube_addBox(&m->leftArm, -1, -2, -2, 4, 12, 4);
    Cube_setPos(&m->leftArm, 5, 2, 0);

    // right leg
    Cube_init(&m->rightLeg, 0, 16);
    Cube_addBox(&m->rightLeg, -2, 0, -2, 4, 12, 4);
    Cube_setPos(&m->rightLeg, -2, 12, 0);

    // left leg
    Cube_init(&m->leftLeg, 0, 16);
    Cube_addBox(&m->leftLeg, -2, 0, -2, 4, 12, 4);
    Cube_setPos(&m->leftLeg, 2, 12, 0);
}

void ZombieModel_render(const ZombieModel* m, double time) {
    // animate
    ((Cube*)&m->head)->yRot      = (float)sin(time * 0.83);
    ((Cube*)&m->head)->xRot      = (float)sin(time) * 0.8f;
    ((Cube*)&m->rightArm)->xRot  = (float)sin(time * 0.6662 + M_PI) * 2.0f;
    ((Cube*)&m->rightArm)->zRot  = (float)(sin(time * 0.2312) + 1.0);
    ((Cube*)&m->leftArm)->xRot   = (float)sin(time * 0.6662) * 2.0f;
    ((Cube*)&m->leftArm)->zRot   = (float)(sin(time * 0.2812) - 1.0);
    ((Cube*)&m->rightLeg)->xRot  = (float)sin(time * 0.6662) * 1.4f;
    ((Cube*)&m->leftLeg)->xRot   = (float)sin(time * 0.6662 + M_PI) * 1.4f;

    // draw
    Cube_render(&m->head);
    Cube_render(&m->body);
    Cube_render(&m->rightArm);
    Cube_render(&m->leftArm);
    Cube_render(&m->rightLeg);
    Cube_render(&m->leftLeg);
}
