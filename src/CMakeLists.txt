add_subdirectory(nes)
add_subdirectory(pixel_engine)

add_executable(
    ${CMAKE_PROJECT_NAME}
    main.cpp
#    Emulator.cpp
#    EmulatorWindow.cpp
)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ocfbnj::nes
    ocfbnj::pixel_engine
    glfw::glfw
    glad::glad
)
