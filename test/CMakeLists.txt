if(GTest_FOUND OR TARGET CONAN_PKG::sfml)
    if(NOT USE_CONAN)
        set(DEPS GTest::gtest GTest::gtest_main)
    else()
        set(DEPS CONAN_PKG::gtest)
    endif()

    set(HEADERS
        ${CMAKE_SOURCE_DIR}/Bus.h
        ${CMAKE_SOURCE_DIR}/Cartridge.h
        ${CMAKE_SOURCE_DIR}/CPU.h
        ${CMAKE_SOURCE_DIR}/literals.h
        ${CMAKE_SOURCE_DIR}/Mapper.h
        ${CMAKE_SOURCE_DIR}/Mapper0.h
        ${CMAKE_SOURCE_DIR}/NesFile.h
        ${CMAKE_SOURCE_DIR}/PPU.h)

    set(SRCS
        testCPU.cpp
        ${CMAKE_SOURCE_DIR}/Bus.cpp
        ${CMAKE_SOURCE_DIR}/Cartridge.cpp
        ${CMAKE_SOURCE_DIR}/CPU.cpp
        ${CMAKE_SOURCE_DIR}/Mapper.cpp
        ${CMAKE_SOURCE_DIR}/Mapper0.cpp
        ${CMAKE_SOURCE_DIR}/NesFile.cpp
        ${CMAKE_SOURCE_DIR}/PPU.cpp)

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/nestest.nes
              ${CMAKE_CURRENT_SOURCE_DIR}/nestest.txt
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    add_executable(testCPU ${HEADERS} ${SRCS})
    target_link_libraries(testCPU ${DEPS})
    target_include_directories(testCPU PRIVATE ${CMAKE_SOURCE_DIR})
    target_compile_definitions(testCPU PRIVATE NES_DEBUG)
endif()
