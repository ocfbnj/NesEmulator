#ifndef OCFBNJ_PIXEL_ENGINE_VAO_H
#define OCFBNJ_PIXEL_ENGINE_VAO_H

#include <optional>

#include <glad/glad.h>

class VBO;

class VAO {
public:
    VAO();

    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;
    VAO(VAO&&) = default;
    VAO& operator=(VAO&&) = default;

    ~VAO();

    void linkAttrib(VBO& vbo, GLuint layout, GLint size, GLenum type, GLsizei stride, void* offset);

    void bind();
    void unBind();

private:
    std::optional<GLuint> id;
};

#endif // OCFBNJ_PIXEL_ENGINE_VAO_H
