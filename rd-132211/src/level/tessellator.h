#ifndef TESSELLATOR_H
#define TESSELLATOR_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>

#define MAX_VERTICES 100000

typedef struct {
    float vertexBuffer[MAX_VERTICES * 3];
    float textureCoordinateBuffer[MAX_VERTICES * 2];
    float colorBuffer[MAX_VERTICES * 3];

    int vertices;

    // Texture
    int hasTexture;
    float textureU;
    float textureV;

    // Color
    bool hasColor;
    float red;
    float green;
    float blue;
} Tessellator;

extern Tessellator TESSELLATOR;

void Tessellator_init(Tessellator* tessellator);
void Tessellator_vertex(Tessellator* tessellator, float x, float y, float z);
void Tessellator_texture(Tessellator* tessellator, float textureU, float textureV);
void Tessellator_color(Tessellator* tessellator, float red, float green, float blue);
void Tessellator_flush(Tessellator* tessellator);
void Tessellator_clear(Tessellator* tessellator);

#endif  // TESSELLATOR_H