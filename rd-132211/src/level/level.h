// level/level.h

#ifndef LEVEL_H_
#define LEVEL_H_

class Level {
public:
    int width;
    int height;
    int depth;

    Level(int width, int height, int depth);
    bool isTile(int x, int y, int z);
    bool isSolidTile(int x, int y, int z);

private:
    signed char* blocks;
};

#endif // LEVEL_H_