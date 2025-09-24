// hitresult.c — block raycast result

#include "hitresult.h"

void hitresult_create(HitResult* h, int x, int y, int z, int o, int f) {
    h->x = x; h->y = y; h->z = z;
    h->o = o; h->f = f;
}