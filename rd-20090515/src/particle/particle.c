// particle/particle.c

#include "particle.h"
#include <math.h>
#include <stdlib.h>

static inline double rnd01(void){ return (double)rand() / (double)RAND_MAX; }

void Particle_init(Particle* p, Level* level,
                   double x, double y, double z,
                   double motionX, double motionY, double motionZ,
                   int textureId) {
    Entity_init(&p->base, level);
    p->textureId = textureId;

    Entity_setSize(&p->base, 0.2f, 0.2f);
    p->base.heightOffset = p->base.boundingBoxHeight / 2.0f;

    Entity_setPosition(&p->base, x, y, z);

    // init jitter
    p->base.motionX = motionX + ((((double)rand() / (double)RAND_MAX) * 2.0) - 1.0) * 0.4;
    p->base.motionY = motionY + ((((double)rand() / (double)RAND_MAX) * 2.0) - 1.0) * 0.4;
    p->base.motionZ = motionZ + ((((double)rand() / (double)RAND_MAX) * 2.0) - 1.0) * 0.4;

    // speed
    double speed = (((double)rand() / (double)RAND_MAX)
                + ((double)rand() / (double)RAND_MAX) + 1.0) * 0.15;

    double d = sqrt(p->base.motionX*p->base.motionX +
                    p->base.motionY*p->base.motionY +
                    p->base.motionZ*p->base.motionZ);
    if (d < 1e-8) d = 1.0;
    p->base.motionX = p->base.motionX / d * speed * 0.7;
    p->base.motionY = p->base.motionY / d * speed;
    p->base.motionZ = p->base.motionZ / d * speed * 0.7;

    p->textureUOffset = (float)rnd01() * 3.0f;
    p->textureVOffset = (float)rnd01() * 3.0f;
}

void Particle_onTick(Particle* p) {
    Entity_onTick(&p->base);

    // 10% kill chance in onTick
    if (((double)rand() / (double)RAND_MAX) < 0.1) {
        Entity_remove(&p->base);
        return;
    }

    p->base.motionY -= 0.06;

    Entity_move(&p->base, p->base.motionX, p->base.motionY, p->base.motionZ);

    p->base.motionX *= 0.98;
    p->base.motionY *= 0.98;
    p->base.motionZ *= 0.98;

    if (p->base.onGround) {
        p->base.motionX *= 0.7;
        p->base.motionZ *= 0.7;
    }
}

void Particle_render(const Particle* p, Tessellator* t, float partialTicks,
                     float cameraX, float cameraY, float cameraZ) {
    float minU = ( (p->textureId % 16) + p->textureUOffset / 4.0f ) / 16.0f;
    float maxU = minU + 999.0f / 64000.0f;
    float minV = ( (p->textureId / 16) + p->textureVOffset / 4.0f ) / 16.0f;
    float maxV = minV + 999.0f / 64000.0f;

    float x = (float)(p->base.prevX + (p->base.x - p->base.prevX) * partialTicks);
    float y = (float)(p->base.prevY + (p->base.y - p->base.prevY) * partialTicks);
    float z = (float)(p->base.prevZ + (p->base.z - p->base.prevZ) * partialTicks);

    const float size = 0.1f;

    Tessellator_vertexUV(t, x - cameraX * size, y - cameraY * size, z - cameraZ * size, minU, maxV);
    Tessellator_vertexUV(t, x - cameraX * size, y + cameraY * size, z - cameraZ * size, minU, minV);
    Tessellator_vertexUV(t, x + cameraX * size, y + cameraY * size, z + cameraZ * size, maxU, minV);
    Tessellator_vertexUV(t, x + cameraX * size, y - cameraY * size, z + cameraZ * size, maxU, maxV);
}
