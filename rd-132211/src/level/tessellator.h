// level/tessellator.h

#ifndef TESSELLATOR_H_
#define TESSELLATOR_H_

#include <iostream>
#include <GL/glew.h>

class Tessellator {
public:
    Tessellator();
    ~Tessellator();
    void init();
    void vertex(float x, float y, float z);
    void flush();
    void clear();

private:
    GLfloat* vertexBuffer;
    int vertices;
    int bufferSize;
};

#endif // TESSELLATOR_H_