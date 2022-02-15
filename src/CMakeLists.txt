add_subdirectory(nes)
add_subdirectory(pixel_engine)
add_subdirectory(audio_maker)

add_executable(
    ${CMAKE_PROJECT_NAME}
    main.cpp
    Emulator.cpp
)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ocfbnj::nes
    ocfbnj::pixel_engine
    ocfbnj::audio_maker
    MbedTLS::mbedtls
)
