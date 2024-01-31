#include <stdlib.h>
#include <math.h>
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