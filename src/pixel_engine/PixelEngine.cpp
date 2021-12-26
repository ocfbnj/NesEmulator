#include <cassert>
#include <thread>

#include "PixelEngine.h"

using namespace std::chrono_literals;

namespace {
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

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

template <class Clock, class Duration>
void sleepUntil(const std::chrono::time_point<Clock, Duration>& absTime) {
#ifdef _WIN32
    // TODO On my machine, using `std::this_thread::sleep_until()` is not precise?
    while (Clock::now() < absTime) {
        std::this_thread::yield();
    }
#else
    std::this_thread::sleep_until(absTime);
#endif
}
} // namespace

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

    setFpsLimit(0);
    setVsyncEnabled(false);

    fpsUpdateInterval = 500ms;
}

PixelEngine::~PixelEngine() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void PixelEngine::run() {
    startTime = Clock::now();

    onBegin();

    while (!glfwWindowShouldClose(window)) {
        updateFps();
        glfwPollEvents();
        onUpdate();
        render();

        Clock::time_point now = Clock::now();
        Clock::duration elapsedTime = now - startTime;
        startTime = now;

        if (frameTimeLimit != 0s) {
            Clock::time_point sleepEnd = now + (frameTimeLimit - elapsedTime);
            sleepUntil(sleepEnd);

            startTime = sleepEnd;
        }
    }

    onEnd();
}

void PixelEngine::setFpsLimit(int value) {
    if (value > 0) {
        frameTimeLimit = std::chrono::duration_cast<Clock::duration>(1s) / value;
    } else {
        frameTimeLimit = 0s;
    }
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
    static Clock::time_point lastFpsUpdate;
    static Clock::time_point tp;

    Clock::time_point now = Clock::now();

    if (now - lastFpsUpdate >= fpsUpdateInterval) {
        lastFpsUpdate = now;

        Clock::rep fps = 1s / (now - tp);
        glfwSetWindowTitle(window, (title + " [FPS: " + std::to_string(fps) + "]").data());
    }

    tp = now;
}
