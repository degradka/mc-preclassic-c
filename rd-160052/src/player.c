// player.c — player base

#include "player.h"
#include <math.h>

void Player_init(Player* p, Level* level) {
    Entity_init(&p->e, level);
    p->e.heightOffset = 1.62f;
}

void Player_turn(Player* p, GLFWwindow* window, float dx, float dy) {
    Entity_turn(&p->e, dx, dy);
    // consume mouse deltas by resetting cursor to the origin each frame
    glfwSetCursorPos(window, 0, 0);
}

void Player_onTick(Player* p, GLFWwindow* window) {
    Entity_onTick(&p->e);

    float forward = 0.0f, strafe = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        Entity_resetPosition(&p->e);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) forward -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) forward += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) strafe  -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) strafe  += 1.0f;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && p->e.onGround) {
        p->e.motionY = 0.5f;
    }

    Entity_moveRelative(&p->e, strafe, forward, p->e.onGround ? 0.1f : 0.02f);

    p->e.motionY -= 0.08f; // gravity

    Entity_move(&p->e, p->e.motionX, p->e.motionY, p->e.motionZ);

    p->e.motionX *= 0.91f;
    p->e.motionY *= 0.98f;
    p->e.motionZ *= 0.91f;

    if (p->e.onGround) {
        p->e.motionX *= 0.7f;
        p->e.motionZ *= 0.7f;
    }
}