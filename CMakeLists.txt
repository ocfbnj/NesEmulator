cmake_minimum_required(VERSION 3.1.2)

project(NesEmulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

option(BUILD_TEST "Build the test program" ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
else()
    message(
        WARNING
            "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first"
    )
endif()

add_subdirectory(src)

# TODO
if(BUILD_TEST)
    # add_subdirectory(test)
endif()
