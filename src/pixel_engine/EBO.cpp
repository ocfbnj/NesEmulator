#include "EBO.h"

EBO::EBO(GLuint* indices, GLsizeiptr size) {
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);

    id = ebo;
}

EBO::~EBO() {
    if (id.has_value()) {
        glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &id.value());
    }
}

void EBO::bind() {
    if (id.has_value()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id.value());
    }
}

void EBO::unBind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
