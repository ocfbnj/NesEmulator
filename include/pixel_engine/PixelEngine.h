#ifndef OCFBNJ_PIXEL_ENGINE_H
#define OCFBNJ_PIXEL_ENGINE_H

#include <atomic>
#include <chrono>
#include <mutex>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <pixel_engine/EBO.h>
#include <pixel_engine/Pixel.h>
#include <pixel_engine/Shader.h>
#include <pixel_engine/Texture.h>
#include <pixel_engine/VAO.h>
#include <pixel_engine/VBO.h>

struct GLFWwindow;

class PixelEngine {
public:
    enum class KeyStatus {
        Press,
        Release
    };

    // clang-format off
    enum class Key {
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Space,
        Enter,
    };
    // clang-format on

    PixelEngine(int width, int height, std::string_view title, int scale);
    virtual ~PixelEngine() = default;

    void run();

    void setFpsLimit(int value);
    void setFpsUpdateInterval(int ms);
    void setVsyncEnabled(bool enabled);

    Pixel getPixel(int x, int y) const;
    void drawPixel(int x, int y, Pixel pixel);
    void drawPixels(std::span<const std::uint8_t> rawPixels);

    KeyStatus getKey(Key key) const;

    virtual void onBegin();
    virtual void onUpdate();
    virtual void onEnd();

    virtual void onSize(int width, int height);
    virtual void onMaximize(bool maximized);
    virtual void onIconify(bool iconified);
    virtual void onFocus(bool focused);
    virtual void onRefresh();

private:
    using Clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
                                     std::chrono::high_resolution_clock,
                                     std::chrono::steady_clock>;

    void userThread();

    void setWindowTitle(std::string_view str);

    void render();
    void updateFps();

    struct GLContext {
        GLContext(int width, int height, std::string_view title);
        ~GLContext();

        GLFWwindow* window;
    };

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void windowSizeCallback(GLFWwindow* window, int width, int height);
    static void windowMaximizeCallback(GLFWwindow* window, int maximized);
    static void windowIconifyCallback(GLFWwindow* window, int iconified);
    static void windowFocusCallback(GLFWwindow* window, int focused);
    static void windowRefreshCallback(GLFWwindow* window);

    int width;
    int height;

    std::string title;

    GLContext glContext;

    Shader shader;
    VAO vao;
    VBO vbo;
    EBO ebo;

    std::vector<Pixel> pixels;
    Texture texture;

    Clock::time_point startTime;

    Clock::duration frameTimeLimit;
    Clock::duration fpsUpdateInterval;

    bool shoundRendering = true;
    std::mutex mtx;
    std::condition_variable cond;
    std::atomic<bool> exit = false;
};

#endif // OCFBNJ_PIXEL_ENGINE_H
