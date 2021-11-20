#ifndef NESEMULATOR_EBO_H
#define NESEMULATOR_EBO_H

#include <optional>

#include <glad/glad.h>

class EBO {
public:
    EBO(GLuint* indices, GLsizeiptr size);
    EBO(const EBO& other) = delete;
    EBO(EBO&& other) noexcept;
    ~EBO();

    void bind();
    void unBind();

private:
    std::optional<GLuint> id;
};

#endif // NESEMULATOR_EBO_H
