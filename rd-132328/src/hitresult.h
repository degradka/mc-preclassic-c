// hitresult.h — block raycast result

#ifndef HITRESULT_H
#define HITRESULT_H

typedef struct HitResult {
    int x, y, z; // block position
    int o;       // unused/original offset field (kept for parity)
    int f;       // face 0..5
} HitResult;

void hitresult_create(HitResult* hitResult, int x, int y, int z, int o, int f);

#endif