// level/tessellator.cpp

#include "tessellator.h"

Tessellator::Tessellator() : vertexBuffer(nullptr), vertices(0), bufferSize(300000) {
    vertexBuffer = new GLfloat[bufferSize];
}

Tessellator::~Tessellator() {
    delete[] vertexBuffer;
}

void Tessellator::init() {
    clear();
}

void Tessellator::vertex(float x, float y, float z) {
    // Ensure there is enough space in the buffer
    if (vertices * 3 + 3 > bufferSize) {
        std::cerr << "Tessellator buffer overflow!" << std::endl;
        return;
    }

    // Vertex
    vertexBuffer[vertices * 3] = x;
    vertexBuffer[vertices * 3 + 1] = y;
    vertexBuffer[vertices * 3 + 2] = z;

    vertices++;
}

void Tessellator::flush() {
    if (vertices == 0) {
        return; // Nothing to render
    }

    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertexBuffer);
    glDrawArrays(GL_QUADS, 0, vertices);

    glDisableClientState(GL_VERTEX_ARRAY);

    // Reset after rendering
    clear();
}

void Tessellator::clear() {
    vertices = 0;
}
