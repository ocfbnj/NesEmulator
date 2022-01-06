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
const std::unordered_map<int, PixelEngine::Key> keyMap{
    {GLFW_KEY_UNKNOWN, PixelEngine::Key::Unknown},
    {GLFW_KEY_SPACE, PixelEngine::Key::Space},
    {GLFW_KEY_APOSTROPHE, PixelEngine::Key::Apostrophe},
    {GLFW_KEY_COMMA, PixelEngine::Key::Comma},
    {GLFW_KEY_MINUS, PixelEngine::Key::Minus},
    {GLFW_KEY_PERIOD, PixelEngine::Key::Period},
    {GLFW_KEY_SLASH, PixelEngine::Key::Slash},
    {GLFW_KEY_0, PixelEngine::Key::Num0},
    {GLFW_KEY_1, PixelEngine::Key::Num1},
    {GLFW_KEY_2, PixelEngine::Key::Num2},
    {GLFW_KEY_3, PixelEngine::Key::Num3},
    {GLFW_KEY_4, PixelEngine::Key::Num4},
    {GLFW_KEY_5, PixelEngine::Key::Num5},
    {GLFW_KEY_6, PixelEngine::Key::Num6},
    {GLFW_KEY_7, PixelEngine::Key::Num7},
    {GLFW_KEY_8, PixelEngine::Key::Num8},
    {GLFW_KEY_9, PixelEngine::Key::Num9},
    {GLFW_KEY_SEMICOLON, PixelEngine::Key::Semicolon},
    {GLFW_KEY_EQUAL, PixelEngine::Key::Equal},
    {GLFW_KEY_A, PixelEngine::Key::A},
    {GLFW_KEY_B, PixelEngine::Key::B},
    {GLFW_KEY_C, PixelEngine::Key::C},
    {GLFW_KEY_D, PixelEngine::Key::D},
    {GLFW_KEY_E, PixelEngine::Key::E},
    {GLFW_KEY_F, PixelEngine::Key::F},
    {GLFW_KEY_G, PixelEngine::Key::G},
    {GLFW_KEY_H, PixelEngine::Key::H},
    {GLFW_KEY_I, PixelEngine::Key::I},
    {GLFW_KEY_J, PixelEngine::Key::J},
    {GLFW_KEY_K, PixelEngine::Key::K},
    {GLFW_KEY_L, PixelEngine::Key::L},
    {GLFW_KEY_M, PixelEngine::Key::M},
    {GLFW_KEY_N, PixelEngine::Key::N},
    {GLFW_KEY_O, PixelEngine::Key::O},
    {GLFW_KEY_P, PixelEngine::Key::P},
    {GLFW_KEY_Q, PixelEngine::Key::Q},
    {GLFW_KEY_R, PixelEngine::Key::R},
    {GLFW_KEY_S, PixelEngine::Key::S},
    {GLFW_KEY_T, PixelEngine::Key::T},
    {GLFW_KEY_U, PixelEngine::Key::U},
    {GLFW_KEY_V, PixelEngine::Key::V},
    {GLFW_KEY_W, PixelEngine::Key::W},
    {GLFW_KEY_X, PixelEngine::Key::X},
    {GLFW_KEY_Y, PixelEngine::Key::Y},
    {GLFW_KEY_Z, PixelEngine::Key::Z},
    {GLFW_KEY_LEFT_BRACKET, PixelEngine::Key::LeftBracket},
    {GLFW_KEY_BACKSLASH, PixelEngine::Key::Backslash},
    {GLFW_KEY_RIGHT_BRACKET, PixelEngine::Key::RightBracket},
    {GLFW_KEY_GRAVE_ACCENT, PixelEngine::Key::GraveAccent},
    {GLFW_KEY_WORLD_1, PixelEngine::Key::World1},
    {GLFW_KEY_WORLD_2, PixelEngine::Key::World2},
    {GLFW_KEY_ESCAPE, PixelEngine::Key::Escape},
    {GLFW_KEY_ENTER, PixelEngine::Key::Enter},
    {GLFW_KEY_TAB, PixelEngine::Key::Tab},
    {GLFW_KEY_BACKSPACE, PixelEngine::Key::Backspace},
    {GLFW_KEY_INSERT, PixelEngine::Key::Insert},
    {GLFW_KEY_DELETE, PixelEngine::Key::Delete},
    {GLFW_KEY_RIGHT, PixelEngine::Key::Right},
    {GLFW_KEY_LEFT, PixelEngine::Key::Left},
    {GLFW_KEY_DOWN, PixelEngine::Key::Down},
    {GLFW_KEY_UP, PixelEngine::Key::Up},
    {GLFW_KEY_PAGE_UP, PixelEngine::Key::PageUp},
    {GLFW_KEY_PAGE_DOWN, PixelEngine::Key::PageDown},
    {GLFW_KEY_HOME, PixelEngine::Key::Home},
    {GLFW_KEY_END, PixelEngine::Key::End},
    {GLFW_KEY_CAPS_LOCK, PixelEngine::Key::CapsLock},
    {GLFW_KEY_SCROLL_LOCK, PixelEngine::Key::ScrollLock},
    {GLFW_KEY_NUM_LOCK, PixelEngine::Key::NumLock},
    {GLFW_KEY_PRINT_SCREEN, PixelEngine::Key::PrintScreen},
    {GLFW_KEY_PAUSE, PixelEngine::Key::Pause},
    {GLFW_KEY_F1, PixelEngine::Key::F1},
    {GLFW_KEY_F2, PixelEngine::Key::F2},
    {GLFW_KEY_F3, PixelEngine::Key::F3},
    {GLFW_KEY_F4, PixelEngine::Key::F4},
    {GLFW_KEY_F5, PixelEngine::Key::F5},
    {GLFW_KEY_F6, PixelEngine::Key::F6},
    {GLFW_KEY_F7, PixelEngine::Key::F7},
    {GLFW_KEY_F8, PixelEngine::Key::F8},
    {GLFW_KEY_F9, PixelEngine::Key::F9},
    {GLFW_KEY_F10, PixelEngine::Key::F10},
    {GLFW_KEY_F11, PixelEngine::Key::F11},
    {GLFW_KEY_F12, PixelEngine::Key::F12},
    {GLFW_KEY_F13, PixelEngine::Key::F13},
    {GLFW_KEY_F14, PixelEngine::Key::F14},
    {GLFW_KEY_F15, PixelEngine::Key::F15},
    {GLFW_KEY_F16, PixelEngine::Key::F16},
    {GLFW_KEY_F17, PixelEngine::Key::F17},
    {GLFW_KEY_F18, PixelEngine::Key::F18},
    {GLFW_KEY_F19, PixelEngine::Key::F19},
    {GLFW_KEY_F20, PixelEngine::Key::F20},
    {GLFW_KEY_F21, PixelEngine::Key::F21},
    {GLFW_KEY_F22, PixelEngine::Key::F22},
    {GLFW_KEY_F23, PixelEngine::Key::F23},
    {GLFW_KEY_F24, PixelEngine::Key::F24},
    {GLFW_KEY_F25, PixelEngine::Key::F25},
    {GLFW_KEY_KP_0, PixelEngine::Key::Kp0},
    {GLFW_KEY_KP_1, PixelEngine::Key::Kp1},
    {GLFW_KEY_KP_2, PixelEngine::Key::Kp2},
    {GLFW_KEY_KP_3, PixelEngine::Key::Kp3},
    {GLFW_KEY_KP_4, PixelEngine::Key::Kp4},
    {GLFW_KEY_KP_5, PixelEngine::Key::Kp5},
    {GLFW_KEY_KP_6, PixelEngine::Key::Kp6},
    {GLFW_KEY_KP_7, PixelEngine::Key::Kp7},
    {GLFW_KEY_KP_8, PixelEngine::Key::Kp8},
    {GLFW_KEY_KP_9, PixelEngine::Key::Kp9},
    {GLFW_KEY_KP_DECIMAL, PixelEngine::Key::KpDecimal},
    {GLFW_KEY_KP_DIVIDE, PixelEngine::Key::KpDivide},
    {GLFW_KEY_KP_MULTIPLY, PixelEngine::Key::KpMultiply},
    {GLFW_KEY_KP_SUBTRACT, PixelEngine::Key::KpSubtract},
    {GLFW_KEY_KP_ADD, PixelEngine::Key::KpAdd},
    {GLFW_KEY_KP_ENTER, PixelEngine::Key::KpEnter},
    {GLFW_KEY_KP_EQUAL, PixelEngine::Key::KpEqual},
    {GLFW_KEY_LEFT_SHIFT, PixelEngine::Key::LeftShift},
    {GLFW_KEY_LEFT_CONTROL, PixelEngine::Key::LeftControl},
    {GLFW_KEY_LEFT_ALT, PixelEngine::Key::LeftAlt},
    {GLFW_KEY_LEFT_SUPER, PixelEngine::Key::LeftSuper},
    {GLFW_KEY_RIGHT_SHIFT, PixelEngine::Key::RightShift},
    {GLFW_KEY_RIGHT_CONTROL, PixelEngine::Key::RightControl},
    {GLFW_KEY_RIGHT_ALT, PixelEngine::Key::RightAlt},
    {GLFW_KEY_RIGHT_SUPER, PixelEngine::Key::RightSuper},
    {GLFW_KEY_MENU, PixelEngine::Key::Menu},
};

const std::unordered_map<int, PixelEngine::KeyStatus> statusMap{
    {GLFW_PRESS, PixelEngine::KeyStatus::Press},
    {GLFW_RELEASE, PixelEngine::KeyStatus::Release},
    {GLFW_REPEAT, PixelEngine::KeyStatus::Repeat},
};

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
      texture(pixels.data(), width, height),
      frameTimeLimit(0s),
      fpsUpdateInterval(500ms),
      exit(false),
      needRendering(true),
      mainThreadId(std::this_thread::get_id()) {
    vao.linkAttrib(vbo, 0, 2, GL_FLOAT, 4 * sizeof(float), reinterpret_cast<void*>(0 * sizeof(float)));
    vao.linkAttrib(vbo, 1, 2, GL_FLOAT, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

    setVsyncEnabled(false);
    bindCallback();
}

void PixelEngine::run() {
    onBegin();

    std::thread t{&PixelEngine::userThread, this};

    while (!glfwWindowShouldClose(glContext.window)) {
        mainThreadQueue.pull();

        if (needRendering) {
            needRendering = false;
            render();
            updateFps();
        }

        glfwPollEvents();
    }

    exit = true;

    if (t.joinable()) {
        t.join();
    }

    userThreadId = std::thread::id{};

    onEnd();
}

void PixelEngine::setFpsLimit(int value) {
    if (std::this_thread::get_id() != mainThreadId) {
        runInMainThread([this, value] { setFpsLimit(value); });
        return;
    }

    assertInMainThread();

    if (value > 0) {
        frameTimeLimit = std::chrono::duration_cast<Clock::duration>(1s) / value;
    } else {
        frameTimeLimit = 0s;
    }
}

void PixelEngine::setFpsUpdateInterval(int ms) {
    if (std::this_thread::get_id() != mainThreadId) {
        runInMainThread([this, ms] { setFpsUpdateInterval(ms); });
    }

    assertInMainThread();

    if (ms > 0) {
        fpsUpdateInterval = std::chrono::milliseconds{ms};
    } else {
        fpsUpdateInterval = 0ms;
    }
}

void PixelEngine::setVsyncEnabled(bool enabled) {
    runInMainThread([this, enabled] {
        assertInMainThread();
        glfwSwapInterval(enabled ? 1 : 0);
    });
}

void PixelEngine::setWindowTitle(const std::string& str) {
    runInMainThread([this, str] {
        assertInMainThread();
        glfwSetWindowTitle(glContext.window, str.data());
    });
}

Pixel PixelEngine::getPixel(int x, int y) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);

    y = height - y - 1;

    std::lock_guard lock{mtx};
    return pixels[y * width + x];
}

void PixelEngine::drawPixel(int x, int y, Pixel pixel) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);

    y = height - y - 1;

    std::lock_guard lock{mtx};
    pixels[y * width + x] = pixel;
}

void PixelEngine::drawPixels(std::span<const std::uint8_t> rawPixels) {
    std::lock_guard lock{mtx};

    assert(rawPixels.size() == pixels.size() * sizeof(decltype(pixels)::value_type));
    std::memcpy(pixels.data(), rawPixels.data(), rawPixels.size());
}

void PixelEngine::onBegin() {
    assertInMainThread();
}

void PixelEngine::onUpdate() {
    assertInUserThread();
}

void PixelEngine::onEnd() {
    assertInMainThread();
}

void PixelEngine::onKeyPress(Key key) {
    assertInUserThread();
}

void PixelEngine::onKeyRelease(Key key) {
    assertInUserThread();
}

void PixelEngine::onKeyRepeat(Key key) {
    assertInUserThread();
}

void PixelEngine::onSize(int width, int height) {
    assertInUserThread();
}

void PixelEngine::onMaximize(bool maximized) {
    assertInUserThread();
}

void PixelEngine::onIconify(bool iconified) {
    assertInUserThread();
}

void PixelEngine::onFocus(bool focused) {
    assertInUserThread();
}

void PixelEngine::onRefresh() {
    assertInUserThread();
}

void PixelEngine::bindCallback() {
    glfwSetWindowUserPointer(glContext.window, this);

    glfwSetKeyCallback(glContext.window, PixelEngine::keyCallback);

    glfwSetFramebufferSizeCallback(glContext.window, PixelEngine::framebufferSizeCallback);
    glfwSetWindowSizeCallback(glContext.window, PixelEngine::windowSizeCallback);
    glfwSetWindowMaximizeCallback(glContext.window, PixelEngine::windowMaximizeCallback);
    glfwSetWindowIconifyCallback(glContext.window, PixelEngine::windowIconifyCallback);
    glfwSetWindowFocusCallback(glContext.window, PixelEngine::windowFocusCallback);
    glfwSetWindowRefreshCallback(glContext.window, PixelEngine::windowRefreshCallback);
}

void PixelEngine::userThread() {
    userThreadId = std::this_thread::get_id();

    startTime = Clock::now();

    while (!exit) {
        userThreadQueue.pull();

        onUpdate();
        needRendering = true;

        Clock::time_point now = Clock::now();
        Clock::duration elapsedTime = now - startTime;
        startTime = now;

        if (frameTimeLimit != 0s) {
            Clock::time_point sleepEnd = now + (frameTimeLimit - elapsedTime);
            sleepUntil(sleepEnd);

            startTime = sleepEnd;
        }
    }
}

void PixelEngine::render() {
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.activate();
    texture.bind();
    vao.bind();

    {
        std::lock_guard lock{mtx};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    }

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

void PixelEngine::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto pixelEngine = static_cast<PixelEngine*>(glfwGetWindowUserPointer(window));

    auto it1 = keyMap.find(key);
    auto it2 = statusMap.find(action);

    if (it1 != keyMap.end() && it2 != statusMap.end()) {
        Key aKey = it1->second;
        KeyStatus status = it2->second;

        switch (status) {
        case KeyStatus::Press:
            pixelEngine->runInUserThread([=] { pixelEngine->onKeyPress(aKey); });
            break;
        case KeyStatus::Release:
            pixelEngine->runInUserThread([=] { pixelEngine->onKeyRelease(aKey); });
            break;
        case KeyStatus::Repeat:
            pixelEngine->runInUserThread([=] { pixelEngine->onKeyRepeat(aKey); });
            break;
        default:
            assert(0);
            break;
        }
    }
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

    pixelEngine->runInMainThread([=] { glViewport(x, y, w, h); });
}

void PixelEngine::windowSizeCallback(GLFWwindow* window, int width, int height) {
    auto pixelEngine = static_cast<PixelEngine*>(glfwGetWindowUserPointer(window));
    pixelEngine->runInUserThread([=] { pixelEngine->onSize(width, height); });
}

void PixelEngine::windowMaximizeCallback(GLFWwindow* window, int maximized) {
    auto pixelEngine = static_cast<PixelEngine*>(glfwGetWindowUserPointer(window));
    pixelEngine->runInUserThread([=] { pixelEngine->onMaximize(maximized); });
}

void PixelEngine::windowIconifyCallback(GLFWwindow* window, int iconified) {
    auto pixelEngine = static_cast<PixelEngine*>(glfwGetWindowUserPointer(window));
    pixelEngine->runInUserThread([=] { pixelEngine->onIconify(iconified); });
}

void PixelEngine::windowFocusCallback(GLFWwindow* window, int focused) {
    auto pixelEngine = static_cast<PixelEngine*>(glfwGetWindowUserPointer(window));
    pixelEngine->runInUserThread([=] { pixelEngine->onFocus(focused); });
}

void PixelEngine::windowRefreshCallback(GLFWwindow* window) {
    auto pixelEngine = static_cast<PixelEngine*>(glfwGetWindowUserPointer(window));
    pixelEngine->runInMainThread([=] { pixelEngine->render(); });
    pixelEngine->runInUserThread([=] { pixelEngine->onRefresh(); });
}

void PixelEngine::runInMainThread(TaskQueue::Task task) {
    if (std::this_thread::get_id() == mainThreadId) {
        task();
        return;
    }

    mainThreadQueue.push(std::move(task));
}

void PixelEngine::runInUserThread(TaskQueue::Task task) {
    if (std::this_thread::get_id() == userThreadId) {
        task();
        return;
    }

    userThreadQueue.push(std::move(task));
}

void PixelEngine::assertInMainThread() {
    assert(std::this_thread::get_id() == mainThreadId);
}

void PixelEngine::assertInUserThread() {
    assert(std::this_thread::get_id() == userThreadId);
}
