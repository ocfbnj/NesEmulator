cmake_minimum_required(VERSION 3.5)

project(NesEmulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########### Conan Package Manager ###########################################################
#############################################################################################

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/release/0.17/conan.cmake"
         "${CMAKE_BINARY_DIR}/conan.cmake"
         EXPECTED_HASH SHA256=3bef79da16c2e031dc429e1dac87a08b9226418b300ce004cc125a82687baeef
         TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

# Add requires here
conan_cmake_configure(
    REQUIRES
        gtest/cci.20210126
        glfw/3.3.6
        glad/0.1.34
        openal/1.21.1
        mbedtls/3.1.0
    GENERATORS
        cmake_find_package
    IMPORTS
        "bin, *.dll -> ./src"
        "lib, *.dylib* -> ./src"
)

conan_cmake_autodetect(settings)
conan_cmake_install(
    PATH_OR_REFERENCE .
    BUILD missing
    REMOTE conancenter
    SETTINGS ${settings}
)

# Add `find_package` here
find_package(GTest)
find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)
find_package(OpenAL REQUIRED)
find_package(MbedTLS REQUIRED)

#############################################################################################
########### Conan Package Manager End #######################################################

add_subdirectory(src)
add_subdirectory(test)
