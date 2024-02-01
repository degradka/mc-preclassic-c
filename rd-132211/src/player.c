#include "player.h"

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

// Pitch limit
const float maxPitch = 90.0F;
const float minPitch = -90.0F;

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

    Player_setPosition(player, x, y, z);
}

void Player_turn(Player* player, GLFWwindow* window, float x, float y) {
    player->yRotation += x * 0.15F;
    player->xRotation -= y * 0.15F;

    if (player->xRotation > maxPitch) player->xRotation = maxPitch;
    if (player->xRotation < minPitch) player->xRotation = minPitch;

    // Reset mouse motion after using it
    glfwSetCursorPos(window, 0, 0);
}

void Player_move(Player* player, double x, double y, double z) {
    double prevX = x;
    double prevY = y;
    double prevZ = z;

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

    // Update on ground state
    player->onGround = prevY != y && prevY < 0.0F;

    // Stop motion on collision
    if (prevX != x) player->motionX = 0.0;
    if (prevY != y) player->motionY = 0.0;
    if (prevZ != z) player->motionZ = 0.0;

    // Move the actual player position
    player->x = (player->boundingBox.minX + player->boundingBox.maxX) / 2.0;
    player->y = player->boundingBox.minY + 1.62F;
    player->z = (player->boundingBox.minZ + player->boundingBox.maxZ) / 2.0;
}

void Player_tick(Player* player, GLFWwindow* window) {
    float forward = 0.0F;
    float vertical = 0.0F;

    // Player movement using GLFW
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        Player_resetPosition(player);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        forward--;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        forward++;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vertical--;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vertical++;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (player->onGround) {
            player->motionY = 0.12F;
        }
    }

    // Add motion to the player using keyboard input
    Player_moveRelative(player, vertical, forward, player->onGround ? 0.1F : 0.01F);

    // Apply gravity
    player->motionY -= 0.005;

    // Move the player using the motion
    Player_move(player, player->motionX, player->motionY, player->motionZ);
    
    // Decrease motion
    player->motionX *= 0.91F;
    player->motionY *= 0.98F;
    player->motionZ *= 0.91F;

    // Decrease motion on ground
    if (player->onGround) {
        player->motionX *= 0.8F;
        player->motionZ *= 0.8F;
    }
}

void Player_moveRelative(Player* player, float x, float z, float speed) {
    float distance = x * x + z * z;

    // Stop moving if too slow
    if (distance < 0.01F) {
        return;
    }

    // Calculate sin and cos of player rotation only once
    double sin_val = sin(degToRad(player->yRotation));
    double cos_val = cos(degToRad(player->yRotation));

    // Apply speed to relative movement
    float factor = speed / sqrtf(distance);
    x *= factor;
    z *= factor;

    // Move the player in the facing direction
    player->motionX += x * cos_val - z * sin_val;
    player->motionZ += z * cos_val + x * sin_val;
}