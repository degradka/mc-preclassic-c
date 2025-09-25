// level/perlin.h — simple Perlin-like height field

#include "perlin.h"
#include <stdlib.h>

#define FUZZINESS 16

static inline int rnd_range(int max2) { return (rand() % max2); }

void Perlin_read(int width, int height, int octave, int* out) {
    int wh = width * height;
    int* table = (int*)malloc((size_t)wh * sizeof(int));
    if (!table) return;

    // seed coarse grid with random values in [-128..+128] * FUZZINESS
    int step_init = width >> octave;
    for (int y = 0; y < height; y += step_init) {
        for (int x = 0; x < width; x += step_init) {
            table[x + y * width] = ((rnd_range(256) - 128) * FUZZINESS);
        }
    }

    // mutate down to step==2
    for (int step = step_init; step > 1; step /= 2) {
        int max   = 256 * (step << octave);
        int half  = step / 2;

        // First mutation: fill centers (x+half, y+half)
        for (int y = 0; y < height; y += step) {
            for (int x = 0; x < width; x += step) {
                int v   = table[(x % width) + (y % height) * width];
                int vx  = table[((x + step) % width) + (y % height) * width];
                int vy  = table[(x % width) + ((y + step) % height) * width];
                int vxy = table[((x + step) % width) + ((y + step) % height) * width];

                int mutated = (v + vy + vx + vxy) / 4 + (rnd_range(max * 2) - max);
                table[(x + half) + (y + half) * width] = mutated;
            }
        }

        // Second mutation: fill edges (x+half,y) and (x,y+half)
        for (int y = 0; y < height; y += step) {
            for (int x = 0; x < width; x += step) {
                int v  = table[x + y * width];
                int vx = table[((x + step) % width) + y * width];
                int vy = table[x + ((y + step) % height) * width];

                // Java used bitwise wrap with (width-1)/(height-1) — sizes are powers of two
                int hxpos = table[((x + half) & (width  - 1)) + ((y + half - step) & (height - 1)) * width];
                int hypos = table[((x + half - step) & (width - 1)) + ((y + half) & (height - 1)) * width];

                int h  = table[(x + half) % width + ((y + half) % height) * width];

                int mutatedX = (v + vx + h + hxpos) / 4 + (rnd_range(max * 2) - max);
                int mutatedY = (v + vy + h + hypos) / 4 + (rnd_range(max * 2) - max);

                table[(x + half) + y * width] = mutatedX;
                table[x + (y + half) * width] = mutatedY;
            }
        }
    }

    // Output in [0..255]
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            out[x + y * width] = table[(x % width) + (y % height) * width] / 512 + 128;
        }
    }

    free(table);
}
