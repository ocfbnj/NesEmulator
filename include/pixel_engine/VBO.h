#ifndef OCFBNJ_PIXEL_ENGINE_VBO_H
#define OCFBNJ_PIXEL_ENGINE_VBO_H

#include <optional>

#include <glad/glad.h>

class VBO {
public:
    VBO(GLfloat* vertices, GLsizeiptr size);

    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;
    VBO(VBO&& other) = default;
    VBO& operator=(VBO&& other) = default;

    ~VBO();

    void bind();
    void unBind();

private:
    std::optional<GLuint> id;
};

#endif // OCFBNJ_PIXEL_ENGINE_VBO_H
