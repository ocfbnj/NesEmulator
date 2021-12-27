#include <pixel_engine/VAO.h>
#include <pixel_engine/VBO.h>

VAO::VAO() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    id = vao;
}

VAO::~VAO() {
    if (id.has_value()) {
        glDeleteVertexArrays(1, &id.value());
    }
}

void VAO::linkAttrib(VBO& vbo, GLuint layout, GLint size, GLenum type, GLsizei stride, void* offset) {
    vbo.bind();
    glVertexAttribPointer(layout, size, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    vbo.unBind();
}

void VAO::bind() {
    if (id.has_value()) {
        glBindVertexArray(id.value());
    }
}

void VAO::unBind() {
    glBindVertexArray(0);
}
