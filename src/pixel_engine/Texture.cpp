#include "Texture.h"

Texture::Texture(const void* bytes, GLint width, GLint height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    id = texture;
}

Texture::Texture(Texture&& other) noexcept {
    id = other.id;
    other.id.reset();
}

Texture::~Texture() {
    if (id.has_value()) {
        glDeleteTextures(GL_TEXTURE_2D, &id.value());
    }
}

void Texture::bind() {
    if (id.has_value()) {
        glBindTexture(GL_TEXTURE_2D, id.value());
    }
}

void Texture::unBind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
