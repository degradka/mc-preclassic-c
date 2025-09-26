// particle/particle.c

#include "particle.h"
#include <math.h>
#include <stdlib.h>

static inline float frand01(void) { return (float)rand() / (float)RAND_MAX; }

void Particle_init(Particle* p, Level* level,
                   double x, double y, double z,
                   double mx, double my, double mz, int textureId)
{
    Entity_init(&p->base, level);
    p->textureId = textureId;

    // size & bbox
    p->base.boundingBoxWidth  = 0.2f;
    p->base.boundingBoxHeight = 0.2f;
    p->base.heightOffset = p->base.boundingBoxHeight * 0.5f;

    Entity_setPosition(&p->base, x, y, z);

    // initial motion + random jitter
    p->base.motionX = mx + (frand01() * 2.0f - 1.0f) * 0.4f;
    p->base.motionY = my + (frand01() * 2.0f - 1.0f) * 0.4f;
    p->base.motionZ = mz + (frand01() * 2.0f - 1.0f) * 0.4f;

    double speed = (frand01() + frand01() + 1.0) * 0.15;
    double d = sqrt(p->base.motionX*p->base.motionX +
                    p->base.motionY*p->base.motionY +
                    p->base.motionZ*p->base.motionZ);
    if (d > 0.0) {
        p->base.motionX = p->base.motionX / d * speed * 0.7;
        p->base.motionY = p->base.motionY / d * speed;
        p->base.motionZ = p->base.motionZ / d * speed * 0.7;
    }

    p->textureUOffset = frand01() * 3.0f;
    p->textureVOffset = frand01() * 3.0f;

    p->size     = frand01() * 0.5f + 0.5f;
    p->lifetime = (int)(4.0 / (frand01() * 0.9 + 0.1));
    p->age      = 0;
}

void Particle_onTick(Particle* p) {
    Entity_onTick(&p->base);

    if (p->age++ >= p->lifetime) {
        Entity_remove(&p->base);
        return;
    }

    // gravity
    p->base.motionY -= 0.06;

    // move
    Entity_move(&p->base, p->base.motionX, p->base.motionY, p->base.motionZ);

    // damping
    p->base.motionX *= 0.98;
    p->base.motionY *= 0.98;
    p->base.motionZ *= 0.98;

    if (p->base.onGround) {
        p->base.motionX *= 0.7;
        p->base.motionZ *= 0.7;
    }
}

void Particle_render(Particle* p, Tessellator* tess, float partialTicks,
                     float cameraX, float cameraY, float cameraZ,
                     float cameraXWithY, float cameraZWithY)
{
    // UVs
    float minU = ( (p->textureId % 16) + p->textureUOffset * 0.25f ) / 16.0f;
    float maxU = minU + 999.0f / 64000.0f;
    float minV = ( (float)(p->textureId / 16) + p->textureVOffset * 0.25f ) / 16.0f;
    float maxV = minV + 999.0f / 64000.0f;

    // lerped position
    float x = (float)(p->base.prevX + (p->base.x - p->base.prevX) * partialTicks);
    float y = (float)(p->base.prevY + (p->base.y - p->base.prevY) * partialTicks);
    float z = (float)(p->base.prevZ + (p->base.z - p->base.prevZ) * partialTicks);

    float s = p->size * 0.1f;

    Tessellator_vertexUV(tess, x - cameraX * s - cameraXWithY * s, y - cameraY * s, z - cameraZ * s - cameraZWithY * s, minU, maxV);
    Tessellator_vertexUV(tess, x - cameraX * s + cameraXWithY * s, y + cameraY * s, z - cameraZ * s + cameraZWithY * s, minU, minV);
    Tessellator_vertexUV(tess, x + cameraX * s + cameraXWithY * s, y + cameraY * s, z + cameraZ * s + cameraZWithY * s, maxU, minV);
    Tessellator_vertexUV(tess, x + cameraX * s - cameraXWithY * s, y - cameraY * s, z + cameraZ * s - cameraZWithY * s, maxU, maxV);
}