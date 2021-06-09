cmake_minimum_required(VERSION 3.1.2)

project(NesEmulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)

    set(DEPS CONAN_PKG::sfml)
    set(USE_CONAN true)

    file(COPY ${CMAKE_SOURCE_DIR}/test/nestest.nes
              ${CMAKE_SOURCE_DIR}/test/nestest.txt
         DESTINATION ${CMAKE_BINARY_DIR}/bin)

    message("-- Using Conan")
else()
    # vcpkg install sfml
    find_package(
        SFML
        COMPONENTS system window graphics CONFIG
        REQUIRED)

    # vcpkg install gtest
    find_package(GTest CONFIG)

    set(DEPS sfml-system sfml-graphics sfml-window)

    file(COPY ${CMAKE_SOURCE_DIR}/test/nestest.nes
              ${CMAKE_SOURCE_DIR}/test/nestest.txt
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

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
