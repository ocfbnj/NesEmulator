#ifndef NESEMULATOR_VAO_H
#define NESEMULATOR_VAO_H

#include <optional>

#include <glad/glad.h>

class VBO;

class VAO {
public:
    VAO();
    VAO(const VAO& other) = delete;
    VAO(VAO&& other) noexcept;
    ~VAO();

    void linkAttrib(VBO& vbo, GLuint layout, GLint size, GLenum type, GLsizei stride, void* offset);

    void bind();
    void unBind();

private:
    std::optional<GLuint> id;
};

#endif // NESEMULATOR_VAO_H
