#include "tile.h"

Tile grass = { 0 };
Tile rock = { 1 };

void Tile_render(Tile* tile, Tessellator* tessellator, const Level* level, int x, int y, int z) {
    float minX = x + 0.0F;
    float maxX = x + 1.0F;
    float minY = y + 0.0F;
    float maxY = y + 1.0F;
    float minZ = z + 0.0F;
    float maxZ = z + 1.0F;

    // Render bottom face
    if (!Level_isSolidTile(level, x, y - 1, z)) {
        Tessellator_vertex(tessellator, minX, minY, maxZ);
        Tessellator_vertex(tessellator, minX, minY, minZ);
        Tessellator_vertex(tessellator, maxX, minY, minZ);
        Tessellator_vertex(tessellator, maxX, minY, maxZ);
    }

    // Render top face
    if (!Level_isSolidTile(level, x, y + 1, z)) {
        Tessellator_vertex(tessellator, maxX, maxY, maxZ);
        Tessellator_vertex(tessellator, maxX, maxY, minZ);
        Tessellator_vertex(tessellator, minX, maxY, minZ);
        Tessellator_vertex(tessellator, minX, maxY, maxZ);
    }

    // Render side faces Z
    if (!Level_isSolidTile(level, x, y, z - 1)) {
        Tessellator_vertex(tessellator, minX, maxY, minZ);
        Tessellator_vertex(tessellator, maxX, maxY, minZ);
        Tessellator_vertex(tessellator, maxX, minY, minZ);
        Tessellator_vertex(tessellator, minX, minY, minZ);
    }
    if (!Level_isSolidTile(level, x, y, z + 1)) {
        Tessellator_vertex(tessellator, minX, maxY, maxZ);
        Tessellator_vertex(tessellator, minX, minY, maxZ);
        Tessellator_vertex(tessellator, maxX, minY, maxZ);
        Tessellator_vertex(tessellator, maxX, maxY, maxZ);
    }

    // Render side faces X
    if (!Level_isSolidTile(level, x - 1, y, z)) {
        Tessellator_vertex(tessellator, minX, maxY, maxZ);
        Tessellator_vertex(tessellator, minX, maxY, minZ);
        Tessellator_vertex(tessellator, minX, minY, minZ);
        Tessellator_vertex(tessellator, minX, minY, maxZ);
    }
    if (!Level_isSolidTile(level, x + 1, y, z)) {
        Tessellator_vertex(tessellator, maxX, minY, maxZ);
        Tessellator_vertex(tessellator, maxX, minY, minZ);
        Tessellator_vertex(tessellator, maxX, maxY, minZ);
        Tessellator_vertex(tessellator, maxX, maxY, maxZ);
    }
}
