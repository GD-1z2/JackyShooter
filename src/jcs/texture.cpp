#include <glad/glad.h>
#include <jcs/texture.hpp>
#include <stb/stb_image.h>
#include <stdexcept>

uint loadTexture(const std::string &path, bool alpha, bool linear) {
    uint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);

    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    u8 *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (data) {
        int mode = GL_RGB;
        if (4 == channels) mode = GL_RGBA;
        if (1 == channels) mode = GL_RED;

        glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else
        throw std::runtime_error{"Failed to load texture"};

    stbi_image_free(data);

    return texture;
}
