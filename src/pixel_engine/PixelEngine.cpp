#include <cassert>
#include <cmath>
#include <unordered_map>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <pixel_engine/PixelEngine.h>

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

template <class Clock, class Duration>
void sleepUntil(const std::chrono::time_point<Clock, Duration>& absTime) {
#ifdef _WIN32
    // TODO On my machine, using `std::this_thread::sleep_until()` is not precise?
    // The minimum sleep duration is about 15 milliseconds on my Windows 11 machine.
    // See https://github.com/rust-lang/rust/issues/43376

    // But SFML works well?
    // See https://github.com/SFML/SFML/blob/HEAD/src/SFML/System/Sleep.cpp#L39

    while (Clock::now() < absTime) {
        std::this_thread::yield();
    }
#else
    std::this_thread::sleep_until(absTime);
#endif
}
} // namespace

PixelEngine::GLContext::GLContext(int width, int height, std::string_view title) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

PixelEngine::GLContext::~GLContext() {
    glfwMakeContextCurrent(nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}

PixelEngine::PixelEngine(int width, int height, std::string_view title, int scale)
    : width(width),
      height(height),
      title(title),
      glContext(width * scale, height * scale, title),
      shader(),
      vao(),
      vbo(vertices, sizeof vertices),
      ebo(indices, sizeof indices),
      pixels(width * height, Pixel{.r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF}),
      texture(pixels.data(), width, height) {
    vao.linkAttrib(vbo, 0, 2, GL_FLOAT, 4 * sizeof(float), reinterpret_cast<void*>(0 * sizeof(float)));
    vao.linkAttrib(vbo, 1, 2, GL_FLOAT, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

    setFpsLimit(0);
    setFpsUpdateInterval(500);
    setVsyncEnabled(false);

    glfwSetWindowUserPointer(glContext.window, this);
    glfwSetFramebufferSizeCallback(glContext.window, PixelEngine::framebufferSizeCallback);
    glfwSetWindowSizeCallback(glContext.window, PixelEngine::windowSizeCallback);
    glfwSetWindowMaximizeCallback(glContext.window, PixelEngine::windowMaximizeCallback);
    glfwSetWindowIconifyCallback(glContext.window, PixelEngine::windowIconifyCallback);
    glfwSetWindowFocusCallback(glContext.window, PixelEngine::windowFocusCallback);
    glfwSetWindowRefreshCallback(glContext.window, PixelEngine::windowRefreshCallback);
}

void PixelEngine::run() {
    std::thread t{&PixelEngine::userThread, this};

    while (!glfwWindowShouldClose(glContext.window)) {
        glfwPollEvents();
        updateFps();

        {
            std::unique_lock<std::mutex> lock{mtx};
            cond.wait(lock, [this] { return shoundRendering; });
            render();
            shoundRendering = false;
        }
    }

    exit = true;

    if (t.joinable()) {
        t.join();
    }
}

void PixelEngine::setFpsLimit(int value) {
    if (value > 0) {
        frameTimeLimit = std::chrono::duration_cast<Clock::duration>(1s) / value;
    } else {
        frameTimeLimit = 0s;
    }
}

void PixelEngine::setFpsUpdateInterval(int ms) {
    if (ms > 0) {
        fpsUpdateInterval = std::chrono::milliseconds{ms};
    } else {
        fpsUpdateInterval = 0ms;
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

PixelEngine::KeyStatus PixelEngine::getKey(Key key) const {
    static std::unordered_map<Key, int> keyMap{
        {Key::A, GLFW_KEY_A},
        {Key::B, GLFW_KEY_B},
        {Key::C, GLFW_KEY_C},
        {Key::D, GLFW_KEY_D},
        {Key::E, GLFW_KEY_E},
        {Key::F, GLFW_KEY_F},
        {Key::G, GLFW_KEY_G},
        {Key::H, GLFW_KEY_H},
        {Key::I, GLFW_KEY_I},
        {Key::J, GLFW_KEY_J},
        {Key::K, GLFW_KEY_K},
        {Key::L, GLFW_KEY_L},
        {Key::M, GLFW_KEY_M},
        {Key::N, GLFW_KEY_N},
        {Key::O, GLFW_KEY_O},
        {Key::P, GLFW_KEY_P},
        {Key::Q, GLFW_KEY_Q},
        {Key::R, GLFW_KEY_R},
        {Key::S, GLFW_KEY_S},
        {Key::T, GLFW_KEY_T},
        {Key::U, GLFW_KEY_U},
        {Key::V, GLFW_KEY_V},
        {Key::W, GLFW_KEY_W},
        {Key::X, GLFW_KEY_X},
        {Key::Y, GLFW_KEY_Y},
        {Key::Z, GLFW_KEY_Z},
        {Key::Space, GLFW_KEY_SPACE},
        {Key::Enter, GLFW_KEY_ENTER},
    };

    static std::unordered_map<int, KeyStatus> statusMap{
        {GLFW_PRESS, KeyStatus::Press},
        {GLFW_RELEASE, KeyStatus::Release},
    };

    assert(keyMap.contains(key));
    int status = glfwGetKey(glContext.window, keyMap[key]);

    assert(statusMap.contains(status));
    return statusMap[status];
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

void PixelEngine::onSize(int width, int height) {
    // do nothing
}

void PixelEngine::onMaximize(bool maximized) {
    // do nothing
}

void PixelEngine::onIconify(bool iconified) {
    // do nothing
}

void PixelEngine::onFocus(bool focused) {
    // do nothing
}

void PixelEngine::onRefresh() {
    // do nothing
}

void PixelEngine::userThread() {
    startTime = Clock::now();

    onBegin();

    while (!exit) {
        {
            std::unique_lock<std::mutex> lock{mtx};
            onUpdate();
            shoundRendering = true;
        }

        cond.notify_one();

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

void PixelEngine::setWindowTitle(std::string_view str) {
    glfwSetWindowTitle(glContext.window, str.data());
}

void PixelEngine::render() {
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.activate();
    texture.bind();
    vao.bind();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(glContext.window);
}

void PixelEngine::updateFps() {
    static Clock::time_point lastFpsUpdate;
    static Clock::time_point tp;

    Clock::time_point now = Clock::now();

    if (now - lastFpsUpdate >= fpsUpdateInterval) {
        lastFpsUpdate = now;

        float fps = 1s / std::chrono::duration_cast<std::chrono::duration<float>>(now - tp);
        auto displayedFps = static_cast<int>(std::round(fps));
        setWindowTitle(title + " [FPS: " + std::to_string(displayedFps) + "]");
    }

    tp = now;
}

void PixelEngine::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto pixelEngine = static_cast<PixelEngine*>(glfwGetWindowUserPointer(window));

    double ratio = static_cast<double>(width) / height;
    double originRatio = static_cast<double>(pixelEngine->width) / pixelEngine->height;

    int x = 0;
    int y = 0;
    int w = width;
    int h = height;

    if (ratio > originRatio) {
        // according to height
        w = height * originRatio;
        x = (width - w) / 2;
    } else {
        // according to width
        h = width / originRatio;
        y = (height - h) / 2;
    }

    glViewport(x, y, w, h);
}

void PixelEngine::windowSizeCallback(GLFWwindow* window, int width, int height) {
    auto pixelEngine = static_cast<PixelEngine*>(glfwGetWindowUserPointer(window));
    pixelEngine->onSize(width, height);
}

void PixelEngine::windowMaximizeCallback(GLFWwindow* window, int maximized) {
    auto pixelEngine = static_cast<PixelEngine*>(glfwGetWindowUserPointer(window));
    pixelEngine->onMaximize(maximized);
}

void PixelEngine::windowIconifyCallback(GLFWwindow* window, int iconified) {
    auto pixelEngine = static_cast<PixelEngine*>(glfwGetWindowUserPointer(window));
    pixelEngine->onIconify(iconified);
}

void PixelEngine::windowFocusCallback(GLFWwindow* window, int focused) {
    auto pixelEngine = static_cast<PixelEngine*>(glfwGetWindowUserPointer(window));
    pixelEngine->onFocus(focused);
}

void PixelEngine::windowRefreshCallback(GLFWwindow* window) {
    auto pixelEngine = static_cast<PixelEngine*>(glfwGetWindowUserPointer(window));
    pixelEngine->onRefresh();
}
