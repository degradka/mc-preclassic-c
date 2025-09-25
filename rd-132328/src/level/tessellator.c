// level/tessellator.c — client-side batched immediate-mode for quads

#include <string.h>
#include "tessellator.h"

Tessellator TESSELLATOR;

void Tessellator_init(Tessellator* t) {
    Tessellator_clear(t);
}

void Tessellator_vertex(Tessellator* t, float x, float y, float z) {
    t->vertexBuffer[t->vertices * 3 + 0] = x;
    t->vertexBuffer[t->vertices * 3 + 1] = y;
    t->vertexBuffer[t->vertices * 3 + 2] = z;

    if (t->hasTexture) {
        t->texBuffer[t->vertices * 2 + 0] = t->u;
        t->texBuffer[t->vertices * 2 + 1] = t->v;
    }
    if (t->hasColor) {
        t->colorBuffer[t->vertices * 3 + 0] = t->r;
        t->colorBuffer[t->vertices * 3 + 1] = t->g;
        t->colorBuffer[t->vertices * 3 + 2] = t->b;
    }

    t->vertices++;
    if (t->vertices == MAX_VERTICES) {
        Tessellator_flush(t);
    }
}

void Tessellator_texture(Tessellator* t, float u, float v) {
    t->hasTexture = 1;
    t->u = u; t->v = v;
}

void Tessellator_color(Tessellator* t, float r, float g, float b) {
    t->hasColor = 1;
    t->r = r; t->g = g; t->b = b;
}

void Tessellator_flush(Tessellator* t) {
    glVertexPointer(3, GL_FLOAT, 0, t->vertexBuffer);
    glEnableClientState(GL_VERTEX_ARRAY);

    if (t->hasTexture) {
        glTexCoordPointer(2, GL_FLOAT, 0, t->texBuffer);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if (t->hasColor) {
        glColorPointer(3, GL_FLOAT, 0, t->colorBuffer);
        glEnableClientState(GL_COLOR_ARRAY);
    }

    glDrawArrays(GL_QUADS, 0, t->vertices);

    Tessellator_clear(t);
}

void Tessellator_clear(Tessellator* t) {
    // it’s fine not to zero buffers every time; we only reset counters/flags
    t->vertices   = 0;
    t->hasTexture = 0;
    t->hasColor   = 0;
}
