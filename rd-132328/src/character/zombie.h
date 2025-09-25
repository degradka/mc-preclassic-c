// character/zombie.h — simple human model entity

#ifndef CHARACTER_ZOMBIE_H
#define CHARACTER_ZOMBIE_H

#include <GL/glew.h>

typedef struct Zombie {
    float x, y, z;
    float yaw;
} Zombie;

void Zombie_loadTexture(void);
void Zombie_init(Zombie* z, float x, float y, float zpos);
void Zombie_render(const Zombie* z, float partialTicks);
void Zombie_tick(Zombie* z);

#endif /* CHARACTER_ZOMBIE_H */
