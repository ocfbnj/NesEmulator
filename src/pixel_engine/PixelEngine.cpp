#include <thread>

#include "PixelEngine.h"

static GLfloat vertices[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, // lower left corner
    1.0f, -1.0f, 1.0f, 0.0f,  // lower right corner
    1.0f, 1.0f, 1.0f, 1.0f,   // upper right corner
    -1.0f, 1.0f, 0.0f, 1.0f   // upper left corner
};

static GLuint indices[] = {
    0, 1, 2, // lower triangle
    2, 3, 0, // upper triangle
};

static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

PixelEngine::GLLoader::GLLoader(int width, int height, std::string_view title, GLFWwindow** window) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    *window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, framebufferSizeCallback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

PixelEngine::PixelEngine(int width, int height, std::string_view title, int scale)
    : width(width),
      height(height),
      title(title),
      window(nullptr),
      glLoader(width * scale, height * scale, title, &window),
      shader("shaders/default.vert", "shaders/default.frag"),
      vao(),
      vbo(vertices, sizeof vertices),
      ebo(indices, sizeof indices),
      pixels(width * height, Pixel{.r = 255, .g = 255, .b = 255, .a = 255}),
      texture(pixels.data(), width, height) {
    vao.linkAttrib(vbo, 0, 2, GL_FLOAT, 4 * sizeof(float), (void*)(0 * sizeof(float)));
    vao.linkAttrib(vbo, 1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

PixelEngine::~PixelEngine() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void PixelEngine::run() {
    tp = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        frameStart = std::chrono::high_resolution_clock::now();

        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.activate();
        texture.bind();
        vao.bind();

        std::chrono::duration<float, std::micro> elapsedTime = std::chrono::high_resolution_clock::now() - tp;
        onUpdate(elapsedTime.count());
        tp = std::chrono::high_resolution_clock::now();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::chrono::duration<float, std::micro> frameTime = std::chrono::high_resolution_clock::now() - frameStart;
        float realFps = std::chrono::seconds(1) / frameTime;

        if (realFps > fps) {
            realFps = fps;
            std::chrono::duration<float, std::micro> sleepTime =
                std::chrono::microseconds((long long)(1.0f / fps * 1000 * 1000)) - frameTime;
            std::this_thread::sleep_for(sleepTime);
        }

        glfwSetWindowTitle(window, (title + " [FPS: " + std::to_string(realFps) + "]").data());
    }
}

void PixelEngine::onUpdate(float elapsedTime) {
    // do nothing
}

void PixelEngine::drawPixel(int x, int y, Pixel pixel) {
    y = height - y - 1;
    pixels[y * width + x] = pixel;
}
