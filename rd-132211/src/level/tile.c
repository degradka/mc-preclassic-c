#include "tile.h"

Tile grass = { 0 };
Tile rock = { 1 };

void Tile_render(Tile* tile, Tessellator* tessellator, const Level* level, int x, int y, int z) {
    float minU = tile->textureId / 16.0F;
    float maxU = minU + 16 / 256.0F;
    float minV = 0.0F;
    float maxV = minV + 16 / 256.0F;

    float minX = x + 0.0F;
    float maxX = x + 1.0F;
    float minY = y + 0.0F;
    float maxY = y + 1.0F;
    float minZ = z + 0.0F;
    float maxZ = z + 1.0F;

    // Render bottom face
    if (!Level_isSolidTile(level, x, y - 1, z)) {
        Tessellator_texture(tessellator, minU, maxV);
        Tessellator_vertex(tessellator, minX, minY, maxZ);
        Tessellator_texture(tessellator, minU, minV);
        Tessellator_vertex(tessellator, minX, minY, minZ);
        Tessellator_texture(tessellator, maxU, minV);
        Tessellator_vertex(tessellator, maxX, minY, minZ);
        Tessellator_texture(tessellator, maxU, maxV);
        Tessellator_vertex(tessellator, maxX, minY, maxZ);
    }

    // Render top face
    if (!Level_isSolidTile(level, x, y + 1, z)) {
        Tessellator_texture(tessellator, maxU, maxV);
        Tessellator_vertex(tessellator, maxX, maxY, maxZ);
        Tessellator_texture(tessellator, maxU, minV);
        Tessellator_vertex(tessellator, maxX, maxY, minZ);
        Tessellator_texture(tessellator, minU, minV);
        Tessellator_vertex(tessellator, minX, maxY, minZ);
        Tessellator_texture(tessellator, minU, maxV);
        Tessellator_vertex(tessellator, minX, maxY, maxZ);
    }

    // Render side faces Z
    if (!Level_isSolidTile(level, x, y, z - 1)) {
        Tessellator_texture(tessellator, maxU, minV);
        Tessellator_vertex(tessellator, minX, maxY, minZ);
        Tessellator_texture(tessellator, minU, minV);
        Tessellator_vertex(tessellator, maxX, maxY, minZ);
        Tessellator_texture(tessellator, minU, maxV);
        Tessellator_vertex(tessellator, maxX, minY, minZ);
        Tessellator_texture(tessellator, maxU, maxV);
        Tessellator_vertex(tessellator, minX, minY, minZ);
    }
    if (!Level_isSolidTile(level, x, y, z + 1)) {
        Tessellator_texture(tessellator, minU, minV);
        Tessellator_vertex(tessellator, minX, maxY, maxZ);
        Tessellator_texture(tessellator, minU, maxV);
        Tessellator_vertex(tessellator, minX, minY, maxZ);
        Tessellator_texture(tessellator, maxU, maxV);
        Tessellator_vertex(tessellator, maxX, minY, maxZ);
        Tessellator_texture(tessellator, maxU, minV);
        Tessellator_vertex(tessellator, maxX, maxY, maxZ);
    }

    // Render side faces X
    if (!Level_isSolidTile(level, x - 1, y, z)) {
        Tessellator_texture(tessellator, maxU, minV);
        Tessellator_vertex(tessellator, minX, maxY, maxZ);
        Tessellator_texture(tessellator, minU, minV);
        Tessellator_vertex(tessellator, minX, maxY, minZ);
        Tessellator_texture(tessellator, minU, maxV);
        Tessellator_vertex(tessellator, minX, minY, minZ);
        Tessellator_texture(tessellator, maxU, maxV);
        Tessellator_vertex(tessellator, minX, minY, maxZ);
    }
    if (!Level_isSolidTile(level, x + 1, y, z)) {
        Tessellator_texture(tessellator, minU, maxV);
        Tessellator_vertex(tessellator, maxX, minY, maxZ);
        Tessellator_texture(tessellator, maxU, maxV);
        Tessellator_vertex(tessellator, maxX, minY, minZ);
        Tessellator_texture(tessellator, maxU, minV);
        Tessellator_vertex(tessellator, maxX, maxY, minZ);
        Tessellator_texture(tessellator, minU, minV);
        Tessellator_vertex(tessellator, maxX, maxY, maxZ);
    }
}