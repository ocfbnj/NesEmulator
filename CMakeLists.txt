cmake_minimum_required(VERSION 3.0)

project(NesEmulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

if(MSVC)
    # See https://stackoverflow.com/questions/14172856/compile-with-mt-instead-of-md-using-cmake
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE)

    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif(MSVC)

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
    CPU.h
    EmulatorWindow.h
    literals.h
    Mapper.h
    Mapper0.h
    NesFile.h
    PPU.h
    Emulator.h)

set(srcs
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

add_executable(${CMAKE_PROJECT_NAME} ${headers} ${srcs})
target_link_libraries(${CMAKE_PROJECT_NAME} ${deps})
