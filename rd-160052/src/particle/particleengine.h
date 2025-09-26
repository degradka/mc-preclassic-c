// particle/particleengine.h

#ifndef PARTICLE_ENGINE_H
#define PARTICLE_ENGINE_H

#include <GL/glew.h>

struct Level;   typedef struct Level Level;
struct Player;  typedef struct Player Player;

#include "particle.h"
#include "../level/tessellator.h"

typedef struct ParticleEngine {
    Level* level;
    GLuint texture;
    Particle* items;
    int count, capacity;
    Tessellator tess;
} ParticleEngine;

void ParticleEngine_init  (ParticleEngine* pe, Level* level, GLuint terrainTex);
void ParticleEngine_onTick(ParticleEngine* pe);
void ParticleEngine_render(ParticleEngine* pe, const Player* player, float partialTicks, int layer);

void ParticleEngine_add(ParticleEngine* pe, const Particle* p);

#endif