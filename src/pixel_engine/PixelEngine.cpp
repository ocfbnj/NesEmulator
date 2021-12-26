#include <cassert>
#include <cmath>
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
    glfwSwapInterval(0);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

PixelEngine::PixelEngine(int width, int height, std::string_view title, int scale)
    : width(width),
      height(height),
      title(title),
      window(nullptr),
      glLoader(width * scale, height * scale, title, &window),
      shader(),
      vao(),
      vbo(vertices, sizeof vertices),
      ebo(indices, sizeof indices),
      pixels(width * height, Pixel{.r = 255, .g = 255, .b = 255, .a = 255}),
      texture(pixels.data(), width, height) {
    vao.linkAttrib(vbo, 0, 2, GL_FLOAT, 4 * sizeof(float), (void*)(0 * sizeof(float)));
    vao.linkAttrib(vbo, 1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    setFpsLimit(0.0f);
    setVsyncEnabled(false);
}

PixelEngine::~PixelEngine() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void PixelEngine::run() {
    startTime = std::chrono::steady_clock::now();

    onBegin();

    while (!glfwWindowShouldClose(window)) {
        updateFps();
        glfwPollEvents();
        onUpdate();
        render();

        std::chrono::duration<float> elapsedTime = std::chrono::steady_clock::now() - startTime;

        if (fps != 0.0f) {
            std::chrono::duration<float> sleepTime = std::chrono::duration<float>(1.0f / fps) - elapsedTime;
            std::this_thread::sleep_for(sleepTime);
        }

        startTime = std::chrono::steady_clock::now();
    }

    onEnd();
}

void PixelEngine::setFpsLimit(float value) {
    fps = value;
}

void PixelEngine::setVsyncEnabled(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}

Pixel PixelEngine::getPixel(int x, int y) const {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);

    y = height - y - 1;
    return pixels[y * width + x];
}

void PixelEngine::drawPixel(int x, int y, Pixel pixel) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);

    y = height - y - 1;
    pixels[y * width + x] = pixel;
}

void PixelEngine::drawPixels(std::span<const std::uint8_t> rawPixels) {
    assert(rawPixels.size() == pixels.size() * sizeof(decltype(pixels)::value_type));
    std::memcpy(pixels.data(), rawPixels.data(), rawPixels.size());
}

GLFWwindow* PixelEngine::getWindow() {
    return window;
}

void PixelEngine::onBegin() {
    // do nothing
}

void PixelEngine::onUpdate() {
    // do nothing
}

void PixelEngine::onEnd() {
    // do nothing
}

void PixelEngine::render() {
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.activate();
    texture.bind();
    vao.bind();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
}

void PixelEngine::updateFps() {
    static std::chrono::time_point<std::chrono::steady_clock> lastFpsUpdate;
    static std::chrono::time_point<std::chrono::steady_clock> tp;

    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    actualFps = std::chrono::duration<float>{1} / (now - tp);
    tp = now;

    if (now - lastFpsUpdate > fpsUpdateInterval) {
        lastFpsUpdate = now;
        int displayedFps = static_cast<int>(std::round(actualFps));
        glfwSetWindowTitle(window, (title + " [FPS: " + std::to_string(displayedFps) + "]").data());
    }
}
