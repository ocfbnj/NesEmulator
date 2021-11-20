#ifndef NESEMULATOR_VBO_H
#define NESEMULATOR_VBO_H

#include <optional>

#include <glad/glad.h>

class VBO {
public:
    VBO(GLfloat* vertices, GLsizeiptr size);
    VBO(const VBO& other) = delete;
    VBO(VBO&& other) noexcept;
    ~VBO();

    void bind();
    void unBind();

private:
    std::optional<GLuint> id;
};

#endif // NESEMULATOR_VBO_H
