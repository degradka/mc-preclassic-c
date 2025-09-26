// particle/particleengine.c

#include "particleengine.h"
#include <stdlib.h>
#include <math.h>
#include "../textures.h"
#include "../player.h"

static void ensure_capacity(ParticleEngine* pe, int need) {
    if (pe->capacity >= need) return;
    int newcap = pe->capacity ? pe->capacity * 2 : 64;
    if (newcap < need) newcap = need;
    pe->items = (Particle*)realloc(pe->items, (size_t)newcap * sizeof(Particle));
    pe->capacity = newcap;
}

void ParticleEngine_init(ParticleEngine* pe, Level* level, GLuint terrainTex) {
    pe->level = level;
    pe->texture = terrainTex;
    pe->items = NULL;
    pe->count = pe->capacity = 0;
    Tessellator_init(&pe->tess);
}

void ParticleEngine_add(ParticleEngine* pe, const Particle* p) {
    ensure_capacity(pe, pe->count + 1);
    pe->items[pe->count++] = *p;
}

void ParticleEngine_onTick(ParticleEngine* pe) {
    for (int i = 0; i < pe->count; ) {
        Particle_onTick(&pe->items[i]);
        if (pe->items[i].base.removed) {
            pe->items[i] = pe->items[pe->count - 1];
            pe->count--;
        } else {
            i++;
        }
    }
}

void ParticleEngine_render(ParticleEngine* pe, const Player* player, float partialTicks, int layer) {
    if (pe->count == 0) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, pe->texture);

    glDisableClientState(GL_COLOR_ARRAY);
    GLboolean cullWasEnabled = glIsEnabled(GL_CULL_FACE);
    if (cullWasEnabled) glDisable(GL_CULL_FACE);

    // --- NEW camera vectors (match Java commit) ---
    float yawRad   = player->e.yRotation * (float)M_PI / 180.0f;
    float pitchRad = player->e.xRotation * (float)M_PI / 180.0f;

    float cameraX = -cosf(yawRad);
    float cameraY =  cosf(pitchRad);
    float cameraZ = -sinf(yawRad);

    float cameraXWithY = -cameraZ * sinf(pitchRad);
    float cameraZWithY =  cameraX * sinf(pitchRad);
    // ----------------------------------------------

    glColor4f(0.8f, 0.8f, 0.8f, 1.0f);

    Tessellator_init(&pe->tess);
    for (int i = 0; i < pe->count; ++i) {
        Particle* p = &pe->items[i];
        int lit = Entity_isLit(&p->base) ? 1 : 0;
        int inLayer = (layer == 1) ? 1 : 0;
        if ((lit ^ inLayer) == 1) {
            Particle_render(p, &pe->tess, partialTicks,
                            cameraX, cameraY, cameraZ,
                            cameraXWithY, cameraZWithY);
        }
    }
    Tessellator_flush(&pe->tess);

    if (cullWasEnabled) glEnable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
}