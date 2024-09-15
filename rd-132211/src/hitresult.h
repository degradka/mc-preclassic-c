#ifndef HITRESULT_H
#define HITRESULT_H

typedef struct HitResult {
	int x;
	int y;
	int z;
	int o;
	int f;
} HitResult;

void hitresult_create(HitResult* hitResult, int x, int y, int z, int o, int f);

#endif
