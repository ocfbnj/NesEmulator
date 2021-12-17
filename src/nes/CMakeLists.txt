add_library(
    nes
    STATIC
    Bus.cpp
    Cartridge.cpp
    CPU.cpp
    Joypad.cpp
    Mapper.cpp
    Mapper0.cpp
    Mapper1.cpp
    Mapper2.cpp
    Mapper3.cpp
    NesFile.cpp
    PPU.cpp
)
target_include_directories(nes PRIVATE ${CMAKE_SOURCE_DIR}/include/NesEmulator/nes)

add_library(ocfbnj::nes ALIAS nes)
