add_subdirectory(nes)
add_subdirectory(pixel_engine)

add_executable(
    ${CMAKE_PROJECT_NAME}
    main.cpp
    Emulator.cpp
)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ocfbnj::nes
    ocfbnj::pixel_engine
    glfw::glfw
    glad::glad
)

add_executable(
    show_tiles
    show_tiles.cpp
)

target_link_libraries(
    show_tiles
    PRIVATE
    ocfbnj::nes
    ocfbnj::pixel_engine
    glfw::glfw
    glad::glad
)
