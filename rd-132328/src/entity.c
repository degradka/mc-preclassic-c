// entity.c — base physics: movement, camera turn, collision

#include "entity.h"

#define degToRad(d) ((d) * M_PI / 180.0)
#define maxPitch  90.0f
#define minPitch -90.0f

void Entity_init(Entity* e, Level* level) {
    e->level = level;
    Entity_resetPosition(e);
}

void Entity_setPosition(Entity* e, float x, float y, float z) {
    e->x = x; e->y = y; e->z = z;

    const float w = 0.3f, h = 0.9f;
    e->boundingBox = AABB_create(x - w, y - h, z - w, x + w, y + h, z + w);

    e->prevX = x; e->prevY = y; e->prevZ = z;
    e->motionX = e->motionY = e->motionZ = 0.0;
}

void Entity_resetPosition(Entity* e) {
    float x = (float) rand() / (float)RAND_MAX * e->level->width;
    float y = (float) e->level->depth + 3.0f;
    float z = (float) rand() / (float)RAND_MAX * e->level->height;
    Entity_setPosition(e, x, y, z);
}

void Entity_turn(Entity* e, GLFWwindow* window, float dx, float dy) {
    e->yRotation += dx * 0.15f;
    e->xRotation -= dy * 0.15f;

    if (e->xRotation > maxPitch) e->xRotation = maxPitch;
    if (e->xRotation < minPitch) e->xRotation = minPitch;

    // consume mouse deltas by resetting cursor to the origin each frame
    glfwSetCursorPos(window, 0, 0);
}

void Entity_move(Entity* e, double dx, double dy, double dz) {
    const double ox = dx, oy = dy, oz = dz;

    ArrayList_AABB hits = Level_getCubes(e->level, &e->boundingBox);

    for (int i = 0; i < hits.size; i++) dy = AABB_clipYCollide(&hits.aabbs[i], &e->boundingBox, dy);
    AABB_move(&e->boundingBox, 0.0, dy, 0.0);

    for (int i = 0; i < hits.size; i++) dx = AABB_clipXCollide(&hits.aabbs[i], &e->boundingBox, dx);
    AABB_move(&e->boundingBox, dx, 0.0, 0.0);

    for (int i = 0; i < hits.size; i++) dz = AABB_clipZCollide(&hits.aabbs[i], &e->boundingBox, dz);
    AABB_move(&e->boundingBox, 0.0, 0.0, dz);

    e->onGround = (oy != dy) && (oy < 0.0);

    if (ox != dx) e->motionX = 0.0;
    if (oy != dy) e->motionY = 0.0;
    if (oz != dz) e->motionZ = 0.0;

    e->x = (e->boundingBox.minX + e->boundingBox.maxX) * 0.5;
    e->y =  e->boundingBox.minY + 1.62f;
    e->z = (e->boundingBox.minZ + e->boundingBox.maxZ) * 0.5;
}

void Entity_tick(Entity* e, GLFWwindow* window) {
    (void)window;

    e->prevX = e->x;
    e->prevY = e->y;
    e->prevZ = e->z;

    float fwd = 0.0f, strafe = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        Entity_resetPosition(e);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) fwd   -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) fwd   += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) strafe-= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) strafe+= 1.0f;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && e->onGround) {
        e->motionY = 0.12f;
    }

    Entity_moveRelative(e, strafe, fwd, e->onGround ? 0.02f : 0.005f);

    e->motionY -= 0.005; // gravity

    Entity_move(e, e->motionX, e->motionY, e->motionZ);

    e->motionX *= 0.91f;
    e->motionY *= 0.98f;
    e->motionZ *= 0.91f;

    if (e->onGround) {
        e->motionX *= 0.8f;
        e->motionZ *= 0.8f;
    }
}

void Entity_moveRelative(Entity* e, float x, float z, float speed) {
    float d2 = x*x + z*z;
    if (d2 < 0.01f) return;

    const double s = sin(degToRad(e->yRotation));
    const double c = cos(degToRad(e->yRotation));

    float k = speed / sqrtf(d2);
    x *= k; z *= k;

    e->motionX += x * c - z * s;
    e->motionZ += z * c + x * s;
}
