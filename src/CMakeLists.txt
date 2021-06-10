add_subdirectory(nes)

add_executable(${CMAKE_PROJECT_NAME} main.cpp Emulator.cpp EmulatorWindow.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE CONAN_PKG::sfml ocfbnj::nes)
