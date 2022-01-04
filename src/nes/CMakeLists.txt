add_library(
    nes
    STATIC
    APU.cpp
    APU/DMC.cpp
    APU/Envelope.cpp
    APU/LengthCounter.cpp
    APU/Noise.cpp
    APU/Pulse.cpp
    APU/StatusRegister.cpp
    APU/Sweep.cpp
    APU/Timer.cpp
    APU/Triangle.cpp
    Bus.cpp
    Cartridge.cpp
    CPU.cpp
    Joypad.cpp
    literals.cpp
    Mapper.cpp
    Mapper/Mapper0.cpp
    Mapper/Mapper1.cpp
    Mapper/Mapper2.cpp
    Mapper/Mapper3.cpp
    Mapper/Mapper4.cpp
    Mirroring.cpp
    NesFile.cpp
    PPU.cpp
)

target_include_directories(nes PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_library(ocfbnj::nes ALIAS nes)
