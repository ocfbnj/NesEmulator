#ifndef OCFBNJ_PIXEL_ENGINE_EBO_H
#define OCFBNJ_PIXEL_ENGINE_EBO_H

#include <optional>

#include <glad/glad.h>

class EBO {
public:
    EBO(GLuint* indices, GLsizeiptr size);

    EBO(const EBO&) = delete;
    EBO& operator=(const EBO&) = delete;
    EBO(EBO&&) = default;
    EBO& operator=(EBO&&) = default;

    ~EBO();

    void bind();
    void unBind();

private:
    std::optional<GLuint> id;
};

#endif // OCFBNJ_PIXEL_ENGINE_EBO_H
