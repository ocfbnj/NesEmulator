#include "VBO.h"

VBO::VBO(GLfloat* vertices, GLsizeiptr size) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    id = vbo;

}

VBO::VBO(VBO&& other) noexcept {
    id = other.id;
    other.id.reset();
}

VBO::~VBO() {
    if (id.has_value()) {
        glDeleteBuffers(1, &id.value());
    }
}

void VBO::bind() {
    if (id.has_value()) {
        glBindBuffer(GL_ARRAY_BUFFER, id.value());
    }
}

void VBO::unBind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
