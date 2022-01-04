add_library(audio_maker AudioMaker.cpp)

target_include_directories(audio_maker PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(audio_maker PRIVATE OpenAL::OpenAL)

add_library(ocfbnj::audio_maker ALIAS audio_maker)
