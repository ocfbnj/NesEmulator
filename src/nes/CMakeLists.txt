add_library(
    nes
    STATIC
    Bus.cpp
    Cartridge.cpp
    CPU.cpp
    Mapper.cpp
    Mapper0.cpp
    NesFile.cpp
    PPU.cpp
)
target_include_directories(nes PRIVATE ${CMAKE_SOURCE_DIR}/include/NesEmulator/nes)

add_library(ocfbnj::nes ALIAS nes)
