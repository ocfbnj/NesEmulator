#ifndef OCFBNJ_NES_BUS_H
#define OCFBNJ_NES_BUS_H

#include <array>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

#include "CPU.h"
#include "Cartridge.h"
#include "Joypad.h"
#include "Mapper.h"
#include "PPU.h"
#include "literals.h"

// CPU and PPU access memory (including memory-mapped spaces) through the bus.
// This class does not represent the bus itself, it is more like a console.
// Because it contains all the components of the NES system.
class Bus {
public:
    Bus() = default;
    Bus(const Bus&) = delete;
    Bus& operator=(const Bus&) = delete;
    Bus(Bus&&) = default;
    Bus& operator=(Bus&&) = default;

    void insert(Cartridge cartridge);
    void powerUp();

    // CPU and PPU have different buses.

    // CPU read from and write to the MAIN bus.
    std::uint8_t cpuRead(std::uint16_t addr);
    std::uint16_t cpuRead16(std::uint16_t addr);
    void cpuWrite(std::uint16_t addr, std::uint8_t data);

    // PPU read from and write to the PPU bus.
    std::uint8_t ppuRead(std::uint16_t addr);
    void ppuWrite(std::uint16_t addr, std::uint8_t data);

    void clock();
    void reset();

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);

    Mapper& getMapper();
    CPU& getCPU();
    PPU& getPPU();
    Joypad& getJoypad();

private:
    // See https://bugzmanov.github.io/nes_ebook/images/ch2/image_5_motherboard.png
    std::unique_ptr<Mapper> mapper;
    CPU cpu;
    PPU ppu;
    std::array<std::uint8_t, 2_kb> cpuRam{};
    std::array<std::uint8_t, 2_kb> ppuRam{};

    Joypad joypad;
};

#endif // OCFBNJ_NES_BUS_H
