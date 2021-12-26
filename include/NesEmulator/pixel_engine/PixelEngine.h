#ifndef OCFBNJ_PIXEL_ENGINE_H
#define OCFBNJ_PIXEL_ENGINE_H

#include <chrono>
#include <span>
#include <string>
#include <string_view>
#include <vector>

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

class PixelEngine {
public:
    PixelEngine(int width, int height, std::string_view title, int scale);
    ~PixelEngine();

    void run();

    void setFpsLimit(int value);
    void setVsyncEnabled(bool enabled);

    Pixel getPixel(int x, int y) const;
    void drawPixel(int x, int y, Pixel pixel);
    void drawPixels(std::span<const std::uint8_t> rawPixels);

    GLFWwindow* getWindow();

    virtual void onBegin();
    virtual void onUpdate();
    virtual void onEnd();

private:
    using Clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
                                     std::chrono::high_resolution_clock,
                                     std::chrono::steady_clock>;

    void render();
    void updateFps();

    int width;
    int height;

    std::string title;

    GLFWwindow* window;

    struct GLLoader {
        GLLoader(int width, int height, std::string_view title, GLFWwindow** window);
    } glLoader;

    Shader shader;

    VAO vao;
    VBO vbo;
    EBO ebo;

    std::vector<Pixel> pixels;
    Texture texture;

    Clock::time_point startTime;

    Clock::duration frameTimeLimit;
    Clock::duration fpsUpdateInterval;
};

#endif // OCFBNJ_PIXEL_ENGINE_H
