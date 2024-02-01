#include "player.h"

void Player_init(Player* player, Level* level) {
    player->level = level;
    Player_resetPosition(player);
}

void Player_setPosition(Player* player, float x, float y, float z) {
    player->x = x;
    player->y = y;
    player->z = z;
}

void Player_resetPosition(Player* player) {
    //float x = (float) rand() / RAND_MAX * player->level->width;
    //float y = (float) (player->level->depth + 3);
    //float z = (float) rand() / RAND_MAX * player->level->height;

    float x = 64.0;
    float y = 64.0;
    float z = 64.0;

    Player_setPosition(player, x, y, z);
}

void Player_turn(Player* player, float motionX, float motionY) {
    player->yRotation += motionX * 0.15F;
    player->xRotation -= motionY * 0.15F;
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
        player->y++;
    }

    Player_moveRelative(player, motionX, motionZ, 10.0F);
}

void Player_moveRelative(Player* player, float motionX, float motionZ, float speed) {
    float sin_val = (float) sin(player->yRotation);
    float cos_val = (float) cos(player->yRotation);

    player->x += motionX * cos_val - motionZ * sin_val;
    player->z += motionZ * cos_val + motionX * sin_val;
}
