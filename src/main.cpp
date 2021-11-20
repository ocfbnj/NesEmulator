#include <iostream>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "NesEmulator/pixel_engine/EBO.h"
#include "NesEmulator/pixel_engine/Pixel.h"
#include "NesEmulator/pixel_engine/Shader.h"
#include "NesEmulator/pixel_engine/Texture.h"
#include "NesEmulator/pixel_engine/VAO.h"
#include "NesEmulator/pixel_engine/VBO.h"

GLfloat vertices[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, // lower left corner
    1.0f, -1.0f, 1.0f, 0.0f,  // lower right corner
    1.0f, 1.0f, 1.0f, 1.0f,   // upper right corner
    -1.0f, 1.0f, 0.0f, 1.0f   // upper left corner
};

GLuint indices[] = {
    0, 1, 2, // lower triangle
    2, 3, 0, // upper triangle
};

constexpr auto Width = 256;
constexpr auto Height = 240;

void initGlfw() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

int main() {
    initGlfw();

    GLFWwindow* window = glfwCreateWindow(Width, Height, "Nes Emulator", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, Width, Height);

    Shader shader{"shaders/default.vert", "shaders/default.frag"};

    VAO vao;
    VBO vbo{vertices, sizeof vertices};
    EBO ebo{indices, sizeof indices};

    vao.linkAttrib(vbo, 0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
    vao.linkAttrib(vbo, 1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    vao.unBind();
    vbo.unBind();
    ebo.unBind();

    Pixel bytes[Width * Height];
    for (int i = 0; i != Height; i++) {
        int j = 0;
        for (; j != Width / 2; j++) {
            bytes[i * Width + j] = Pixel{.r = 255, .g = 0, .b = 0, .a = 0};
        }

        for (; j != Width; j++) {
            bytes[i * Width + j] = Pixel{.r = 0, .g = 255, .b = 0, .a = 0};
        }
    }
    Texture texture{bytes, Width, Height};

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.activate();
        texture.bind();
        vao.bind();

        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
