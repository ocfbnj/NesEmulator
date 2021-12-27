#ifndef OCFBNJ_PIXEL_ENGINE_TEXTURE_H
#define OCFBNJ_PIXEL_ENGINE_TEXTURE_H

#include <optional>

#include <glad/glad.h>

class Texture {
public:
    Texture(const void* bytes, GLint width, GLint height);

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) = default;
    Texture& operator=(Texture&&) = default;

    ~Texture();

    void bind();
    void unBind();

private:
    std::optional<GLuint> id;
};

#endif // OCFBNJ_PIXEL_ENGINE_TEXTURE_H
