// player.cpp

#include "player.h"

#include <cmath> // Include cmath for Math functions
#include <cstdlib> // Include cstdlib for rand function

// Constructor
Player::Player(Level* level) : level(level) {
    resetPosition();
}

// Set the player to a specific location
void Player::setPosition(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

// Reset the position of the player to a random location on the level
void Player::resetPosition() {
    float x = static_cast<float>(rand()) / RAND_MAX * level->width;
    float y = static_cast<float>(level->depth + 3);
    float z = static_cast<float>(rand()) / RAND_MAX * level->height;

    setPosition(x, y, z);
}

// Turn the camera using motion yaw and pitch
void Player::turn(float motionX, float motionY) {
    yRotation += motionX * 0.15F;
    xRotation -= motionY * 0.15F;
}