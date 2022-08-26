#include "glad/glad.hpp"
#include <jcs/texture.hpp>
#include <stb/stb_image.hpp>
#include <stdexcept>

uint loadTexture(const std::string &path, bool linear) {
    uint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    linear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    linear ? GL_LINEAR : GL_NEAREST);

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

uint loadCubeMap(const std::array<std::string, 6> &faces) {
    uint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    stbi_set_flip_vertically_on_load(false);

    int width, height, channels;
    for (uint i = 0; i < 6; i++) {
        u8 *data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);

        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            stbi_image_free(data);
            throw std::runtime_error{"Failed to load cube texture's face #"
                                     + std::to_string(i)};
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}
