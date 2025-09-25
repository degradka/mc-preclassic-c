// character/zombie.c

#include "zombie.h"
#include "../textures.h"
#include "../timer.h"
#include <GL/glew.h>
#include <math.h>

static int texChar = 0;

void Zombie_init(Zombie* z, Level* level, double x, double y, double zpos) {
    Entity_init(&z->base, level);
    Entity_setPosition(&z->base, x, y, zpos);

    ZombieModel_init(&z->model);

    z->rotation = ((double)rand() / RAND_MAX) * M_PI * 2.0;
    z->rotationMotionFactor = (((double)rand() / RAND_MAX) + 1.0) * 0.01;
    z->timeOffset = (float)(((double)rand() / RAND_MAX) * 1239813.0);
    z->speed = 1.0f;

    if (!texChar) texChar = loadTexture("resources/char.png", GL_NEAREST);
}

void Zombie_onTick(Zombie* z) {
    Entity_onTick(&z->base);

    z->rotation += z->rotationMotionFactor;
    z->rotationMotionFactor *= 0.99;
    z->rotationMotionFactor += ((double)rand() / RAND_MAX - (double)rand() / RAND_MAX)
                             *  (double)rand() / RAND_MAX * (double)rand() / RAND_MAX
                             *  0.009999999776482582;

    const float v = (float)sin(z->rotation);
    const float f = (float)cos(z->rotation);

    if (z->base.onGround && ((double)rand() / RAND_MAX) < 0.08) z->base.motionY = 0.5f;

    Entity_moveRelative(&z->base, v, f, z->base.onGround ? 0.1f : 0.02f);
    z->base.motionY -= 0.08f;

    Entity_move(&z->base, z->base.motionX, z->base.motionY, z->base.motionZ);

    z->base.motionX *= 0.91f;
    z->base.motionY *= 0.98f;
    z->base.motionZ *= 0.91f;

    if (z->base.y < -100.0f) Entity_remove(&z->base);

    if (z->base.onGround) {
        z->base.motionX *= 0.7f;
        z->base.motionZ *= 0.7f;
    }
}

void Zombie_render(const Zombie* z, float partialTicks) {
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    bind(texChar);

    const double t  = (double)getCurrentTimeInNanoseconds() * 1e-9 * 10.0 * z->speed + z->timeOffset;
    const double ix = z->base.prevX + (z->base.x - z->base.prevX) * partialTicks;
    const double iy = z->base.prevY + (z->base.y - z->base.prevY) * partialTicks;
    const double iz = z->base.prevZ + (z->base.z - z->base.prevZ) * partialTicks;

    glTranslated(ix, iy, iz);
    glScalef(1.f, -1.f, 1.f);

    const float size = 7.0f / 120.0f;
    glScalef(size, size, size);

    const double offY = fabs(sin(t * 2.0 / 3.0)) * 5.0 + 23.0;
    glTranslated(0.0, -offY, 0.0);
    glRotated(z->rotation * 180.0 / M_PI + 180.0, 0, 1, 0);

    // Render the extracted model
    ZombieModel_render(&z->model, t);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
