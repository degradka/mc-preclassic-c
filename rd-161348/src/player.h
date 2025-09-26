// player.h — player base

#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"
#include "entity.h"
#include <GLFW/glfw3.h>

typedef struct Player {
    Entity e;
} Player;

void Player_init(Player* player, Level* level);

void Player_onTick(Player* player, GLFWwindow* window);
void Player_turn(Player* player, GLFWwindow* window, float dx, float dy);

#endif  // PLAYER_H