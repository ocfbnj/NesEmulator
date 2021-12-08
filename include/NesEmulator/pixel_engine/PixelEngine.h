#ifndef NESEMULATOR_PIXELENGINE_H
#define NESEMULATOR_PIXELENGINE_H

#include <string_view>
#include <vector>
#include <string>
#include <chrono>

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
    void drawPixel(int x, int y, Pixel pixel);

    virtual void onUpdate(float elapsedTime);

private:
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

    std::chrono::time_point<std::chrono::high_resolution_clock> frameStart;
    std::chrono::time_point<std::chrono::high_resolution_clock> tp;

    float fps = 120;
};

#endif // NESEMULATOR_PIXELENGINE_H
