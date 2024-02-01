#include "tile.h"

Tile grass = { 0 };
Tile rock = { 1 };

void Tile_render(Tile* tile, Tessellator* tessellator, const Level* level, int layer, int x, int y, int z) {
    float minU = tile->textureId / 16.0F;
    float maxU = minU + 16 / 256.0F;
    float minV = 0.0F;
    float maxV = minV + 16 / 256.0F;

    float shadeX = 0.0f;
    float shadeY = 1.0f;
    float shadeZ = 0.8f;

    float minX = x + 0.0F;
    float maxX = x + 1.0F;
    float minY = y + 0.0F;
    float maxY = y + 1.0F;
    float minZ = z + 0.0F;
    float maxZ = z + 1.0F;

    // Render bottom face
    if (!Level_isSolidTile(level, x, y - 1, z)) {
        // Get the brightness of the tile below
        float brightness = Level_getBrightness(level, x, y - 1, z) * shadeY;

        // Don't render face if both conditions are the same (layer != brightness)
        if ((layer == 1) ^ (brightness == shadeY)) {
            Tessellator_color(tessellator, brightness, brightness, brightness);
            Tessellator_texture(tessellator, minU, maxV);
            Tessellator_vertex(tessellator, minX, minY, maxZ);
            Tessellator_texture(tessellator, minU, minV);
            Tessellator_vertex(tessellator, minX, minY, minZ);
            Tessellator_texture(tessellator, maxU, minV);
            Tessellator_vertex(tessellator, maxX, minY, minZ);
            Tessellator_texture(tessellator, maxU, maxV);
            Tessellator_vertex(tessellator, maxX, minY, maxZ);
        }
    }

    // Render top face
    if (!Level_isSolidTile(level, x, y + 1, z)) {
        // Get the brightness of the tile above
        float brightness = Level_getBrightness(level, x, y + 1, z) * shadeY;

        // Don't render face if both conditions are the same (layer != brightness)
        if ((layer == 1) ^ (brightness == shadeY)) {
            Tessellator_color(tessellator, brightness, brightness, brightness);
            Tessellator_texture(tessellator, maxU, maxV);
            Tessellator_vertex(tessellator, maxX, maxY, maxZ);
            Tessellator_texture(tessellator, maxU, minV);
            Tessellator_vertex(tessellator, maxX, maxY, minZ);
            Tessellator_texture(tessellator, minU, minV);
            Tessellator_vertex(tessellator, minX, maxY, minZ);
            Tessellator_texture(tessellator, minU, maxV);
            Tessellator_vertex(tessellator, minX, maxY, maxZ);
        }
    }

    // Render side faces Z
    if (!Level_isSolidTile(level, x, y, z - 1)) {
        // Get the brightness of the tile next to it
        float brightness = Level_getBrightness(level, x, y, z - 1) * shadeZ;

        // Don't render face if both conditions are the same (layer != brightness)
        if ((layer == 1) ^ (brightness == shadeZ)) {
            Tessellator_color(tessellator, brightness, brightness, brightness);
            Tessellator_texture(tessellator, maxU, minV);
            Tessellator_vertex(tessellator, minX, maxY, minZ);
            Tessellator_texture(tessellator, minU, minV);
            Tessellator_vertex(tessellator, maxX, maxY, minZ);
            Tessellator_texture(tessellator, minU, maxV);
            Tessellator_vertex(tessellator, maxX, minY, minZ);
            Tessellator_texture(tessellator, maxU, maxV);
            Tessellator_vertex(tessellator, minX, minY, minZ);
        }
    }
    if (!Level_isSolidTile(level, x, y, z + 1)) {
        // Get the brightness of the tile next to it
        float brightness = Level_getBrightness(level, x, y, z + 1) * shadeZ;

        // Don't render face if both conditions are the same (layer != brightness)
        if ((layer == 1) ^ (brightness == shadeZ)) {
            Tessellator_color(tessellator, brightness, brightness, brightness);
            Tessellator_texture(tessellator, minU, minV);
            Tessellator_vertex(tessellator, minX, maxY, maxZ);
            Tessellator_texture(tessellator, minU, maxV);
            Tessellator_vertex(tessellator, minX, minY, maxZ);
            Tessellator_texture(tessellator, maxU, maxV);
            Tessellator_vertex(tessellator, maxX, minY, maxZ);
            Tessellator_texture(tessellator, maxU, minV);
            Tessellator_vertex(tessellator, maxX, maxY, maxZ);
        }
    }

    // Render side faces X
    if (!Level_isSolidTile(level, x - 1, y, z)) {
        // Get the brightness of the tile next to it
        float brightness = Level_getBrightness(level, x - 1, y, z) * shadeX;

        // Don't render face if both conditions are the same (layer != brightness)
        if ((layer == 1) ^ (brightness == shadeX)) {
            Tessellator_color(tessellator, brightness, brightness, brightness);
            Tessellator_texture(tessellator, maxU, minV);
            Tessellator_vertex(tessellator, minX, maxY, maxZ);
            Tessellator_texture(tessellator, minU, minV);
            Tessellator_vertex(tessellator, minX, maxY, minZ);
            Tessellator_texture(tessellator, minU, maxV);
            Tessellator_vertex(tessellator, minX, minY, minZ);
            Tessellator_texture(tessellator, maxU, maxV);
            Tessellator_vertex(tessellator, minX, minY, maxZ);
        }
    }
    if (!Level_isSolidTile(level, x + 1, y, z)) {
        // Get the brightness of the tile next to it
        float brightness = Level_getBrightness(level, x + 1, y, z) * shadeX;

        // Don't render face if both conditions are the same (layer != brightness)
        if ((layer == 1) ^ (brightness == shadeX)) {
            Tessellator_color(tessellator, brightness, brightness, brightness);
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
}