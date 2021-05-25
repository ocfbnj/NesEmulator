cmake_minimum_required(VERSION 3.0)

project(NesEmulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory(test)

# vcpkg install sfml
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)

set(deps
    sfml-system
    sfml-graphics
    sfml-window)

set(headers
    Bus.h
    Cartridge.h
    Cpu.h
    literals.h
    Mapper.h
    Mapper0.h
    NesFile.h)

set(srcs
    Bus.cpp
    Cartridge.cpp
    Cpu.cpp
    main.cpp
    Mapper.cpp
    Mapper0.cpp
    NesFile.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${headers} ${srcs})
target_link_libraries(${CMAKE_PROJECT_NAME} ${deps})
