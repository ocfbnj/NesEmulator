if(TARGET gtest::gtest)
    add_executable(testCPU testCPU.cpp)
    target_link_libraries(testCPU gtest::gtest ocfbnj::nes)

    file(
        COPY
            ${CMAKE_CURRENT_SOURCE_DIR}/nestest.nes
            ${CMAKE_CURRENT_SOURCE_DIR}/nestest.txt
        DESTINATION
            ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()
