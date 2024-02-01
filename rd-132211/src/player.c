#include "player.h"

void Player_init(Player* player, Level* level) {
    player->level = level;
    Player_resetPosition(player);
}

void Player_setPosition(Player* player, float x, float y, float z) {
    player->x = x;
    player->y = y;
    player->z = z;
    float width = 0.3F;
    float height = 0.9F;
    player->boundingBox = AABB_create(x - width, y - height, z - width, x + width, y + height, z + width);
}

void Player_resetPosition(Player* player) {
    float x = (float) rand() / RAND_MAX * player->level->width;
    float y = (float) (player->level->depth - 29);
    float z = (float) rand() / RAND_MAX * player->level->height;

    //float x = 64.0;
    //float y = 64.0;
    //float z = 64.0;

    Player_setPosition(player, x, y, z);
}

void Player_turn(Player* player, float motionX, float motionY) {
    player->yRotation += motionX * 0.15F;
    player->xRotation -= motionY * 0.15F;
}

void Player_move(Player* player, double x, double y, double z) {
    // Get surrounded tiles
    ArrayList_AABB aABBs = Level_getCubes(player->level, &player->boundingBox);

    // Check for Y collision
    for (int i = 0; i < aABBs.size; i++) {
        y = AABB_clipYCollide(&aABBs.aabbs[i], &player->boundingBox, y);
    }
    AABB_move(&player->boundingBox, 0.0F, y, 0.0F);

    // Check for X collision
    for (int i = 0; i < aABBs.size; i++) {
        x = AABB_clipXCollide(&aABBs.aabbs[i], &player->boundingBox, x);
    }
    AABB_move(&player->boundingBox, x, 0.0F, 0.0F);

    // Check for Z collision
    for (int i = 0; i < aABBs.size; i++) {
        z = AABB_clipZCollide(&aABBs.aabbs[i], &player->boundingBox, z);
    }
    AABB_move(&player->boundingBox, 0.0F, 0.0F, z);

    // Move the actual player position
    player->x = (player->boundingBox.minX + player->boundingBox.maxX) / 2.0F;
    player->y = player->boundingBox.minY + 1.62F;
    player->z = (player->boundingBox.minZ + player->boundingBox.maxZ) / 2.0F;
}

void Player_tick(Player* player, GLFWwindow* window) {
    float motionX = 0.0F;
    float motionZ = 0.0F;

    // Player movement using GLFW
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        Player_resetPosition(player);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        motionZ--;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        motionZ++;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        motionX--;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        motionX++;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        Player_move(player, 0, 1, 0);
    }

    Player_moveRelative(player, motionX, motionZ, 0.2F);

    Player_move(player, 0, -0.5F, 0);
}

void Player_moveRelative(Player* player, float x, float z, float speed) {
    // Calculate sin and cos of player rotation
    float sin_val = (float) sin(player->yRotation);
    float cos_val = (float) cos(player->yRotation);

    // Move the player in facing direction
    double mX = x * cos_val - z * sin_val;
    double mZ = z * cos_val + x * sin_val;

    // Adjust speed
    mX *= speed;
    mZ *= speed;

    Player_move(player, mX, 0, mZ);
}
