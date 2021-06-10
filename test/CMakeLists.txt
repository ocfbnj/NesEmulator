if(TARGET CONAN_PKG::gtest)
    add_executable(testCPU testCPU.cpp)
    target_link_libraries(testCPU CONAN_PKG::gtest ocfbnj::nes)

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/nestest.nes
              ${CMAKE_CURRENT_SOURCE_DIR}/nestest.txt
         DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()
