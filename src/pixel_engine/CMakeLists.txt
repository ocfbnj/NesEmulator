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

target_include_directories(pixel_engine PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(pixel_engine PRIVATE glfw::glfw glad::glad)

add_library(ocfbnj::pixel_engine ALIAS pixel_engine)
