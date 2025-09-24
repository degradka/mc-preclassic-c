// player.c — movement, camera turn, collision

#include "player.h"

#define degToRad(d) ((d) * M_PI / 180.0)
#define maxPitch  90.0f
#define minPitch -90.0f

void Player_init(Player* player, Level* level) {
    player->level = level;
    Player_resetPosition(player);
}

void Player_setPosition(Player* player, float x, float y, float z) {
    player->x = x; player->y = y; player->z = z;

    const float w = 0.3f, h = 0.9f;
    player->boundingBox = AABB_create(x - w, y - h, z - w, x + w, y + h, z + w);

    player->prevX = x; player->prevY = y; player->prevZ = z;
    player->motionX = player->motionY = player->motionZ = 0.0;
}

void Player_resetPosition(Player* player) {
    float x = (float) rand() / (float)RAND_MAX * player->level->width;
    float y = (float) player->level->depth + 3.0f;
    float z = (float) rand() / (float)RAND_MAX * player->level->height;
    Player_setPosition(player, x, y, z);
}

void Player_turn(Player* player, GLFWwindow* window, float dx, float dy) {
    player->yRotation += dx * 0.15f;
    player->xRotation -= dy * 0.15f;

    if (player->xRotation > maxPitch) player->xRotation = maxPitch;
    if (player->xRotation < minPitch) player->xRotation = minPitch;

    // consume mouse deltas by resetting cursor to the origin each frame
    glfwSetCursorPos(window, 0, 0);
}

void Player_move(Player* player, double dx, double dy, double dz) {
    const double ox = dx, oy = dy, oz = dz;

    ArrayList_AABB hits = Level_getCubes(player->level, &player->boundingBox);

    for (int i = 0; i < hits.size; i++) dy = AABB_clipYCollide(&hits.aabbs[i], &player->boundingBox, dy);
    AABB_move(&player->boundingBox, 0.0, dy, 0.0);

    for (int i = 0; i < hits.size; i++) dx = AABB_clipXCollide(&hits.aabbs[i], &player->boundingBox, dx);
    AABB_move(&player->boundingBox, dx, 0.0, 0.0);

    for (int i = 0; i < hits.size; i++) dz = AABB_clipZCollide(&hits.aabbs[i], &player->boundingBox, dz);
    AABB_move(&player->boundingBox, 0.0, 0.0, dz);

    player->onGround = (oy != dy) && (oy < 0.0);

    if (ox != dx) player->motionX = 0.0;
    if (oy != dy) player->motionY = 0.0;
    if (oz != dz) player->motionZ = 0.0;

    player->x = (player->boundingBox.minX + player->boundingBox.maxX) * 0.5;
    player->y =  player->boundingBox.minY + 1.62f;
    player->z = (player->boundingBox.minZ + player->boundingBox.maxZ) * 0.5;
}

void Player_tick(Player* player, GLFWwindow* window) {
    player->prevX = player->x;
    player->prevY = player->y;
    player->prevZ = player->z;

    float fwd = 0.0f, strafe = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        Player_resetPosition(player);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) fwd   -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) fwd   += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) strafe-= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) strafe+= 1.0f;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player->onGround) {
        player->motionY = 0.12f;
    }

    Player_moveRelative(player, strafe, fwd, player->onGround ? 0.02f : 0.005f);

    player->motionY -= 0.005; // gravity

    Player_move(player, player->motionX, player->motionY, player->motionZ);

    player->motionX *= 0.91f;
    player->motionY *= 0.98f;
    player->motionZ *= 0.91f;

    if (player->onGround) {
        player->motionX *= 0.8f;
        player->motionZ *= 0.8f;
    }
}

void Player_moveRelative(Player* player, float x, float z, float speed) {
    float d2 = x*x + z*z;
    if (d2 < 0.01f) return;

    const double s = sin(degToRad(player->yRotation));
    const double c = cos(degToRad(player->yRotation));

    float k = speed / sqrtf(d2);
    x *= k; z *= k;

    player->motionX += x * c - z * s;
    player->motionZ += z * c + x * s;
}
