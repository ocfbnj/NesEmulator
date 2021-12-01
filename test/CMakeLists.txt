if(TARGET GTest::GTest)
    add_executable(testCPU testCPU.cpp)
    target_link_libraries(testCPU GTest::GTest ocfbnj::nes)

    foreach(path ${CMAKE_BINARY_DIR}/src ${CMAKE_CURRENT_BINARY_DIR})
        file(
            COPY
                ${CMAKE_CURRENT_SOURCE_DIR}/nestest.nes
                ${CMAKE_CURRENT_SOURCE_DIR}/nestest.txt
            DESTINATION
                ${path}
        )
    endforeach()
endif()
