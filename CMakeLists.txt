cmake_minimum_required(VERSION 3.0)

project(NesEmulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# vcpkg install sfml
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)

add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
    NesFile.cpp
    Cartridge.cpp
    Mapper.cpp
    Mapper0.cpp
    Bus.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} sfml-system sfml-graphics sfml-window)
