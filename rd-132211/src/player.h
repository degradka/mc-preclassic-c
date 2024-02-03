#ifndef PLAYER_H_
#define PLAYER_H_

#include "level/level.h" // Include the Level header as needed

class Player {
private:
    Level* level;

public:
    float x, y, z;
    float xRotation, yRotation;

    // Constructor
    Player(Level* level);

    // Functions
    void setPosition(float x, float y, float z);
    void resetPosition();
    void turn(float motionX, float motionY);
};

#endif // PLAYER_H_