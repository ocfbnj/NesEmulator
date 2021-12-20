#ifndef OCFBNJ_NES_BUS_H
#define OCFBNJ_NES_BUS_H

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include "CPU.h"
#include "Joypad.h"
#include "Mapper.h"
#include "PPU.h"
#include "literals.h"

// CPU and PPU access memory (including memory-mapped spaces) through the bus.
// This class does not represent the bus itself, it is more like a console.
// Because it contains all the components of the NES system.
class Bus {
public:
    explicit Bus(std::unique_ptr<Mapper> mapper);

    // CPU and PPU have different buses.

    // CPU read from and write to the MAIN bus.
    uint8_t cpuRead(uint16_t addr);
    uint16_t cpuRead16(uint16_t addr);
    void cpuWrite(uint16_t addr, uint8_t data);

    // PPU read from and write to the PPU bus.
    uint8_t ppuRead(uint16_t addr);
    void ppuWrite(uint16_t addr, uint8_t data);

    void clock();
    void reset();

    Mapper& getMapper();
    CPU& getCPU();
    PPU& getPPU();
    Joypad& getJoypad();

private:
    // See https://bugzmanov.github.io/nes_ebook/images/ch2/image_5_motherboard.png
    std::unique_ptr<Mapper> mapper;
    CPU cpu;
    PPU ppu;
    std::array<uint8_t, 2_kb> cpuRam{};
    std::array<uint8_t, 2_kb> ppuRam{};

    Joypad joypad;
};

#endif // OCFBNJ_NES_BUS_H
