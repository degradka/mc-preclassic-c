#ifndef PLAYER_H
#define PLAYER_H

#include "level/level.h"
#include <math.h>
#include <GLFW/glfw3.h>

typedef struct {
    Level* level;
    float x, y, z;
    float yRotation, xRotation;
} Player;

void Player_init(Player* player, Level* level);
void Player_setPosition(Player* player, float x, float y, float z);
void Player_resetPosition(Player* player);
void Player_turn(Player* player, float motionX, float motionY);
void Player_tick(Player* player, GLFWwindow* window);
void Player_moveRelative(Player* player, float motionX, float motionZ, float speed);

#endif  // PLAYER_H