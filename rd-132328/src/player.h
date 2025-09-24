// player.h — thin wrapper over Entity (room for player-specific stuff later)

#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

typedef struct Player {
    Entity e; // composition: Player “is” an Entity for now
} Player;

/* Thin forwards so existing call sites can stay Player_* for now. */
void Player_init(Player* p, Level* level);
void Player_setPosition(Player* p, float x, float y, float z);
void Player_resetPosition(Player* p);
void Player_turn(Player* p, GLFWwindow* window, float dx, float dy);
void Player_move(Player* p, double dx, double dy, double dz);
void Player_tick(Player* p, GLFWwindow* window);
void Player_moveRelative(Player* p, float x, float z, float speed);

#endif  // PLAYER_H
