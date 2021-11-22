#ifndef NESEMULATOR_TEXTURE_H
#define NESEMULATOR_TEXTURE_H

#include <optional>

#include <glad/glad.h>

class Texture {
public:
    Texture(const void* bytes, GLint width, GLint height);
    Texture(const Texture& other) = delete;
    Texture(Texture&& other) noexcept;
    ~Texture();

    Texture& operator=(Texture&& other) noexcept;

    void bind();
    void unBind();

private:
    std::optional<GLuint> id;
};

#endif // NESEMULATOR_TEXTURE_H
