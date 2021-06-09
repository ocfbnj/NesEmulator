cmake_minimum_required(VERSION 3.1.2)

project(NesEmulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# vcpkg install sfml
find_package(
    SFML
    COMPONENTS system window graphics CONFIG
    REQUIRED)

set(DEPS sfml-system sfml-graphics sfml-window)

set(HEADERS
    Bus.h
    Cartridge.h
    CPU.h
    EmulatorWindow.h
    literals.h
    Mapper.h
    Mapper0.h
    NesFile.h
    PPU.h
    Emulator.h)

set(SRCS
    Bus.cpp
    Cartridge.cpp
    CPU.cpp
    EmulatorWindow.cpp
    main.cpp
    Mapper.cpp
    Mapper0.cpp
    NesFile.cpp
    PPU.cpp
    Emulator.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${HEADERS} ${SRCS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${DEPS})

add_subdirectory(test)
