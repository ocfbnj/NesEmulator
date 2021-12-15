add_library(
    pixel_engine
    STATIC
    Pixel.cpp
    PixelEngine.cpp
    Shader.cpp
    VAO.cpp
    VBO.cpp
    EBO.cpp
    Texture.cpp
)

target_include_directories(pixel_engine PRIVATE ${CMAKE_SOURCE_DIR}/include/NesEmulator/pixel_engine)
target_link_libraries(pixel_engine PRIVATE glfw::glfw glad::glad)

add_library(ocfbnj::pixel_engine ALIAS pixel_engine)
