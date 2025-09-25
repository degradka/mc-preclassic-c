// level/perlin.h — simple Perlin-like height field

#ifndef PERLIN_H
#define PERLIN_H

// octave: 0 (coarse) or 1 (finer)
// out must be width*height ints; values returned are in [0..255]
void Perlin_read(int width, int height, int octave, int* out);

#endif