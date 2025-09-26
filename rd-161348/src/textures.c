// textures.c - texture loader

#include "textures.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int loadTexture(const char* path, int filterMode) {
    (void) filterMode;

    GLuint tex = 0;
    glGenTextures(1, &tex);
    if (!tex) { fprintf(stderr, "Failed to generate texture id\n"); return 0; }

    bind((int)tex);

    // --- WRAP
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // --- FILTERS

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int w,h,ch;
    unsigned char* img = stbi_load(path, &w, &h, &ch, STBI_rgb_alpha);
    if (!img) { fprintf(stderr, "Failed to load texture %s\n", path); return 0; }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, img);


    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(img);
    return (int)tex;
}
void bind(int id) {
    glBindTexture(GL_TEXTURE_2D, (GLuint)id);
}