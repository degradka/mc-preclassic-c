// tile.c — block faces, per-face shading, and selection face

#include "tile.h"

Tile grass = { 0 };
Tile rock  = { 1 };

static inline void quad_color_tex(Tessellator* t, float b, float u0, float v0, float u1, float v1) {
    (void)v0; 
    (void)u1;
    Tessellator_color(t, b, b, b);
    Tessellator_texture(t, u0, v1);
}

void Tile_render(Tile* tile, Tessellator* t, const Level* level, int layer, int x, int y, int z) {
    const float minU = tile->textureId / 16.0f;
    const float maxU = minU + 16.0f / 256.0f;
    const float minV = 0.0f;
    const float maxV = minV + 16.0f / 256.0f;

    const float shadeX = 0.6f, shadeY = 1.0f, shadeZ = 0.8f;

    float minX = (float)x, maxX = (float)x + 1.0f;
    float minY = (float)y, maxY = (float)y + 1.0f;
    float minZ = (float)z, maxZ = (float)z + 1.0f;

    // bottom
    if (!Level_isSolidTile(level, x, y - 1, z)) {
        float b = Level_getBrightness(level, x, y - 1, z) * shadeY;
        if ((layer == 1) ^ (b == shadeY)) {
            Tessellator_color(t, b, b, b);
            Tessellator_texture(t, minU, maxV); Tessellator_vertex(t, minX, minY, maxZ);
            Tessellator_texture(t, minU, minV); Tessellator_vertex(t, minX, minY, minZ);
            Tessellator_texture(t, maxU, minV); Tessellator_vertex(t, maxX, minY, minZ);
            Tessellator_texture(t, maxU, maxV); Tessellator_vertex(t, maxX, minY, maxZ);
        }
    }

    // top
    if (!Level_isSolidTile(level, x, y + 1, z)) {
        float b = Level_getBrightness(level, x, y + 1, z) * shadeY;
        if ((layer == 1) ^ (b == shadeY)) {
            Tessellator_color(t, b, b, b);
            Tessellator_texture(t, maxU, maxV); Tessellator_vertex(t, maxX, maxY, maxZ);
            Tessellator_texture(t, maxU, minV); Tessellator_vertex(t, maxX, maxY, minZ);
            Tessellator_texture(t, minU, minV); Tessellator_vertex(t, minX, maxY, minZ);
            Tessellator_texture(t, minU, maxV); Tessellator_vertex(t, minX, maxY, maxZ);
        }
    }

    // -Z
    if (!Level_isSolidTile(level, x, y, z - 1)) {
        float b = Level_getBrightness(level, x, y, z - 1) * shadeZ;
        if ((layer == 1) ^ (b == shadeZ)) {
            Tessellator_color(t, b, b, b);
            Tessellator_texture(t, maxU, minV); Tessellator_vertex(t, minX, maxY, minZ);
            Tessellator_texture(t, minU, minV); Tessellator_vertex(t, maxX, maxY, minZ);
            Tessellator_texture(t, minU, maxV); Tessellator_vertex(t, maxX, minY, minZ);
            Tessellator_texture(t, maxU, maxV); Tessellator_vertex(t, minX, minY, minZ);
        }
    }

    // +Z
    if (!Level_isSolidTile(level, x, y, z + 1)) {
        float b = Level_getBrightness(level, x, y, z + 1) * shadeZ;
        if ((layer == 1) ^ (b == shadeZ)) {
            Tessellator_color(t, b, b, b);
            Tessellator_texture(t, minU, minV); Tessellator_vertex(t, minX, maxY, maxZ);
            Tessellator_texture(t, minU, maxV); Tessellator_vertex(t, minX, minY, maxZ);
            Tessellator_texture(t, maxU, maxV); Tessellator_vertex(t, maxX, minY, maxZ);
            Tessellator_texture(t, maxU, minV); Tessellator_vertex(t, maxX, maxY, maxZ);
        }
    }

    // -X
    if (!Level_isSolidTile(level, x - 1, y, z)) {
        float b = Level_getBrightness(level, x - 1, y, z) * shadeX;
        if ((layer == 1) ^ (b == shadeX)) {
            Tessellator_color(t, b, b, b);
            Tessellator_texture(t, maxU, minV); Tessellator_vertex(t, minX, maxY, maxZ);
            Tessellator_texture(t, minU, minV); Tessellator_vertex(t, minX, maxY, minZ);
            Tessellator_texture(t, minU, maxV); Tessellator_vertex(t, minX, minY, minZ);
            Tessellator_texture(t, maxU, maxV); Tessellator_vertex(t, minX, minY, maxZ);
        }
    }

    // +X
    if (!Level_isSolidTile(level, x + 1, y, z)) {
        float b = Level_getBrightness(level, x + 1, y, z) * shadeX;
        if ((layer == 1) ^ (b == shadeX)) {
            Tessellator_color(t, b, b, b);
            Tessellator_texture(t, minU, maxV); Tessellator_vertex(t, maxX, minY, maxZ);
            Tessellator_texture(t, maxU, maxV); Tessellator_vertex(t, maxX, minY, minZ);
            Tessellator_texture(t, maxU, minV); Tessellator_vertex(t, maxX, maxY, minZ);
            Tessellator_texture(t, minU, minV); Tessellator_vertex(t, maxX, maxY, maxZ);
        }
    }
}

void Face_render(Tessellator* t, int x, int y, int z, int face) {
    float minX = (float)x,     maxX = (float)x + 1.0f;
    float minY = (float)y,     maxY = (float)y + 1.0f;
    float minZ = (float)z,     maxZ = (float)z + 1.0f;

    if (face == 0) { // bottom
        Tessellator_vertex(t, minX, minY, maxZ);
        Tessellator_vertex(t, minX, minY, minZ);
        Tessellator_vertex(t, maxX, minY, minZ);
        Tessellator_vertex(t, maxX, minY, maxZ);
    } else if (face == 1) { // top
        Tessellator_vertex(t, maxX, maxY, maxZ);
        Tessellator_vertex(t, maxX, maxY, minZ);
        Tessellator_vertex(t, minX, maxY, minZ);
        Tessellator_vertex(t, minX, maxY, maxZ);
    } else if (face == 2) { // -Z
        Tessellator_vertex(t, minX, maxY, minZ);
        Tessellator_vertex(t, maxX, maxY, minZ);
        Tessellator_vertex(t, maxX, minY, minZ);
        Tessellator_vertex(t, minX, minY, minZ);
    } else if (face == 3) { // +Z
        Tessellator_vertex(t, minX, maxY, maxZ);
        Tessellator_vertex(t, minX, minY, maxZ);
        Tessellator_vertex(t, maxX, minY, maxZ);
        Tessellator_vertex(t, maxX, maxY, maxZ);
    } else if (face == 4) { // -X
        Tessellator_vertex(t, minX, maxY, maxZ);
        Tessellator_vertex(t, minX, maxY, minZ);
        Tessellator_vertex(t, minX, minY, minZ);
        Tessellator_vertex(t, minX, minY, maxZ);
    } else if (face == 5) { // +X
        Tessellator_vertex(t, maxX, minY, maxZ);
        Tessellator_vertex(t, maxX, minY, minZ);
        Tessellator_vertex(t, maxX, maxY, minZ);
        Tessellator_vertex(t, maxX, maxY, maxZ);
    }
}
