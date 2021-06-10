#include "NesEmulator/nes/Mapper.h"
#include "NesEmulator/nes/NesFile.h"

#include "Emulator.h"

Emulator::Emulator(std::string_view nesFile) : bus(Mapper::create(loadNesFile(nesFile))) {}

void Emulator::run() {
    CPU& cpu = bus.getCPU();
    PPU& ppu = bus.getPPU();

    while (window.isOpen()) {
        window.handleEvent();

        // TODO

        window.draw();
        window.display();
    }
}
