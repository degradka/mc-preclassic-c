#ifndef TEXTURES_H
#define TEXTURES_H

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

int loadTexture(const char* resourceName, int mode);
void bind(int id);

#endif  // TEXTURES_H