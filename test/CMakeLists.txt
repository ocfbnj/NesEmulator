if(TARGET GTest::GTest)
    add_executable(testCPU testCPU.cpp)
    target_link_libraries(testCPU GTest::GTest ocfbnj::nes)

    file(
        COPY
            ${CMAKE_CURRENT_SOURCE_DIR}/nestest.nes
            ${CMAKE_CURRENT_SOURCE_DIR}/nestest.txt
        DESTINATION
            ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

add_executable(testSleep testSleep.cpp)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    target_link_libraries(testSleep PRIVATE winmm)
endif()

add_executable(testAudioMaker testAudioMaker.cpp)
target_link_libraries(testAudioMaker PRIVATE ocfbnj::audio_maker)
