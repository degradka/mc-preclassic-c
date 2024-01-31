#ifndef TESSELLATOR_H
#define TESSELLATOR_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct {
    float vertexBuffer[300000];
    int vertices;
} Tessellator;

extern Tessellator TESSELLATOR;

void Tessellator_init(Tessellator* tessellator);
void Tessellator_vertex(Tessellator* tessellator, float x, float y, float z);
void Tessellator_flush(Tessellator* tessellator);
void Tessellator_clear(Tessellator* tessellator);

#endif  // TESSELLATOR_H