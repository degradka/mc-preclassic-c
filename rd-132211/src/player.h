#ifndef PLAYER_H
#define PLAYER_H

#include "level/level.h"

typedef struct {
    Level* level;
    float x, y, z;
    float xRotation, yRotation;
} Player;

void Player_init(Player* player, Level* level);
void Player_setPosition(Player* player, float x, float y, float z);
void Player_resetPosition(Player* player);
void Player_turn(Player* player, float motionX, float motionY);

#endif  // PLAYER_H