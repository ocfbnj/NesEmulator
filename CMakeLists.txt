cmake_minimum_required(VERSION 3.5)

project(NesEmulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest)
find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)
find_package(OpenAL REQUIRED)
find_package(MbedTLS REQUIRED)

add_subdirectory(src)
add_subdirectory(test)
