#include "textures.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static int lastId = 0;

int loadTexture(const char* resourceName, int mode) {
    // Generate a new texture id
    GLuint textureId;
    glGenTextures(1, &textureId);

    if (textureId == 0) {
        fprintf(stderr, "Failed to generate texture id\n");
        return 0;
    }

    // Bind this texture id
    bind(textureId);

    // Set texture filter mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);

    // Load texture using stb_image
    int width, height, channels;
    unsigned char* image = stbi_load(resourceName, &width, &height, &channels, STBI_rgb_alpha);

    if (!image) {
        fprintf(stderr, "Failed to load texture %s\n", resourceName);
        return 0;
    }

    // Write texture to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // Generate mipmaps
    // TODO: For some reason this throws a segfault so I commented it out
    //glGenerateMipmap(GL_TEXTURE_2D);

    // Free stb_image data
    stbi_image_free(image);

    return textureId;
}

void bind(int id) {
    if (id != lastId) {
        glBindTexture(GL_TEXTURE_2D, id);
        lastId = id;
    }
}
