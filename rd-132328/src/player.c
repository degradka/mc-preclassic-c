// player.c — thin wrapper over Entity

#include "player.h"

void Player_init(Player* p, Level* level)                               { Entity_init(&p->e, level); }
void Player_setPosition(Player* p, float x, float y, float z)            { Entity_setPosition(&p->e, x, y, z); }
void Player_resetPosition(Player* p)                                     { Entity_resetPosition(&p->e); }
void Player_turn(Player* p, GLFWwindow* window, float dx, float dy)      { Entity_turn(&p->e, window, dx, dy); }
void Player_move(Player* p, double dx, double dy, double dz)             { Entity_move(&p->e, dx, dy, dz); }
void Player_tick(Player* p, GLFWwindow* window)                          { Entity_tick(&p->e, window); }
void Player_moveRelative(Player* p, float x, float z, float speed)       { Entity_moveRelative(&p->e, x, z, speed); }
