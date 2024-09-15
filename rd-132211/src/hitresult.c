#include "hitresult.h"

#include <stdlib.h>

void hitresult_create(HitResult* hitResult, int x, int y, int z, int o, int f) {
    hitResult->x = x;
    hitResult->y = y;
    hitResult->z = z;
    hitResult->o = o;
    hitResult->f = f;
}
