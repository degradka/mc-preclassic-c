#include <string.h>

#include "tessellator.h"

Tessellator TESSELLATOR;

void Tessellator_init(Tessellator* tessellator) {
    Tessellator_clear(tessellator);
}

void Tessellator_vertex(Tessellator* tessellator, float x, float y, float z) {
    // Vertex
    tessellator->vertexBuffer[tessellator->vertices * 3] = x;
    tessellator->vertexBuffer[tessellator->vertices * 3 + 1] = y;
    tessellator->vertexBuffer[tessellator->vertices * 3 + 2] = z;

    tessellator->vertices++;
}

void Tessellator_flush(Tessellator* tessellator) {
    // Flip the buffer
    glVertexPointer(3, GL_FLOAT, 0, tessellator->vertexBuffer);
    glEnableClientState(GL_VERTEX_ARRAY);

    glDrawArrays(GL_QUADS, 0, tessellator->vertices);
    glDisableClientState(GL_VERTEX_ARRAY);

    // Reset after rendering
    Tessellator_clear(tessellator);
}

void Tessellator_clear(Tessellator* tessellator) {
    memset(tessellator->vertexBuffer, 0, sizeof(tessellator->vertexBuffer));
    tessellator->vertices = 0;
}
