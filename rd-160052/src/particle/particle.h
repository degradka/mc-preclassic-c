// particle/particle.h

#ifndef PARTICLE_H
#define PARTICLE_H

#include "../entity.h"
#include "../level/level.h"
#include "../level/tessellator.h"

typedef struct {
    Entity base;
    int    textureId;
    float  textureUOffset;
    float  textureVOffset;
} Particle;

void Particle_init(Particle* p, Level* level,
                   double x, double y, double z,
                   double motionX, double motionY, double motionZ,
                   int textureId);

void Particle_onTick(Particle* p);

void Particle_render(const Particle* p, Tessellator* t, float partialTicks,
                     float cameraX, float cameraY, float cameraZ);

#endif