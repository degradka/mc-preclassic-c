// tessellator.h — client-side batched immediate-mode for quads

#ifndef TESSELLATOR_H
#define TESSELLATOR_H

#include <GL/glew.h>
#include <stdbool.h>

#define MAX_VERTICES 100000

typedef struct {
    float vertexBuffer[MAX_VERTICES * 3];
    float texBuffer   [MAX_VERTICES * 2];
    float colorBuffer [MAX_VERTICES * 3];

    int   vertices;

    // current attribs
    int   hasTexture; float u, v;
    int   hasColor;   float r, g, b;
} Tessellator;

extern Tessellator TESSELLATOR;

void Tessellator_init   (Tessellator* t);
void Tessellator_vertex (Tessellator* t, float x, float y, float z);
void Tessellator_texture(Tessellator* t, float u, float v);
void Tessellator_color  (Tessellator* t, float r, float g, float b);
void Tessellator_flush  (Tessellator* t);
void Tessellator_clear  (Tessellator* t);

#endif  // TESSELLATOR_H
