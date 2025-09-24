// player.h — movement, camera turn, collision

#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"

#include "level/level.h"
#include "phys/aabb.h"
#include <math.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

typedef struct Player {
    Level* level;
    float  x, y, z;
    double prevX, prevY, prevZ;
    double motionX, motionY, motionZ;
    float  yRotation, xRotation;
    bool   onGround;
    AABB   boundingBox;
} Player;

void Player_init(Player* player, Level* level);
void Player_setPosition(Player* player, float x, float y, float z);
void Player_resetPosition(Player* player);
void Player_turn(Player* player, GLFWwindow* window, float x, float y);
void Player_move(Player* player, double x, double y, double z);
void Player_tick(Player* player, GLFWwindow* window);
void Player_moveRelative(Player* player, float x, float y, float speed);

#endif  // PLAYER_H