# vcpkg install gtest
find_package(GTest CONFIG)

if(GTest_FOUND)
    set(deps
        GTest::gtest
        GTest::gtest_main)

    set(headers
        ${CMAKE_SOURCE_DIR}/Bus.h
        ${CMAKE_SOURCE_DIR}/Cartridge.h
        ${CMAKE_SOURCE_DIR}/CPU.h
        ${CMAKE_SOURCE_DIR}/literals.h
        ${CMAKE_SOURCE_DIR}/Mapper.h
        ${CMAKE_SOURCE_DIR}/Mapper0.h
        ${CMAKE_SOURCE_DIR}/NesFile.h
        ${CMAKE_SOURCE_DIR}/PPU.h)

    set(srcs
        testCPU.cpp
        ${CMAKE_SOURCE_DIR}/Bus.cpp
        ${CMAKE_SOURCE_DIR}/Cartridge.cpp
        ${CMAKE_SOURCE_DIR}/CPU.cpp
        ${CMAKE_SOURCE_DIR}/Mapper.cpp
        ${CMAKE_SOURCE_DIR}/Mapper0.cpp
        ${CMAKE_SOURCE_DIR}/NesFile.cpp
        ${CMAKE_SOURCE_DIR}/PPU.cpp)

    file(COPY
            ${CMAKE_CURRENT_SOURCE_DIR}/nestest.nes
            ${CMAKE_CURRENT_SOURCE_DIR}/nestest.txt
         DESTINATION
            ${CMAKE_CURRENT_BINARY_DIR}/)

    add_executable(testCPU ${headers} ${srcs})
    target_link_libraries(testCPU ${deps})
    target_include_directories(testCPU PRIVATE ${CMAKE_SOURCE_DIR})
    target_compile_definitions(testCPU PRIVATE NES_DEBUG)
endif(GTest_FOUND)
