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

    // Texture coordinates
    if (tessellator->hasTexture) {
        tessellator->textureCoordinateBuffer[tessellator->vertices * 2] = tessellator->textureU;
        tessellator->textureCoordinateBuffer[tessellator->vertices * 2 + 1] = tessellator->textureV;
    }

    tessellator->vertices++;

    // Flush if there are too many vertices in the buffer
    if (tessellator->vertices == MAX_VERTICES) {
        Tessellator_flush(tessellator);
    }
}

void Tessellator_texture(Tessellator* tessellator, float textureU, float textureV) {
    tessellator->hasTexture = 1;
    tessellator->textureU = textureU;
    tessellator->textureV = textureV;
}

void Tessellator_flush(Tessellator* tessellator) {
    // Flip the buffer
    glVertexPointer(3, GL_FLOAT, 0, tessellator->vertexBuffer);
    glEnableClientState(GL_VERTEX_ARRAY);

    if (tessellator->hasTexture) {
        glTexCoordPointer(2, GL_FLOAT, 0, tessellator->textureCoordinateBuffer);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glDrawArrays(GL_QUADS, 0, tessellator->vertices);

    // Reset after rendering
    Tessellator_clear(tessellator);
}

void Tessellator_clear(Tessellator* tessellator) {
    memset(tessellator->vertexBuffer, 0, sizeof(tessellator->vertexBuffer));
    memset(tessellator->textureCoordinateBuffer, 0, sizeof(tessellator->textureCoordinateBuffer));
    tessellator->vertices = 0;
    tessellator->hasTexture = 0;
}
