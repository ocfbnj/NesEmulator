#ifndef OCFBNJ_PIXEL_ENGINE_H
#define OCFBNJ_PIXEL_ENGINE_H

#include <chrono>
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

    Pixel getPixel(int x, int y) const;
    void drawPixel(int x, int y, Pixel pixel);

    GLFWwindow* getWindow();

    virtual void onBegin();
    virtual bool onUpdate(float elapsedTime);
    virtual void onEnd();

private:
    void render();

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

    std::chrono::time_point<std::chrono::steady_clock> frameStart;
    std::chrono::time_point<std::chrono::steady_clock> lastUserUpdate;
    std::chrono::time_point<std::chrono::steady_clock> lastFpsUpdate;

    std::chrono::duration<float> fpsUpdateInterval = std::chrono::milliseconds{500};
};

#endif // OCFBNJ_PIXEL_ENGINE_H
