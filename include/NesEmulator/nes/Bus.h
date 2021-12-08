#ifndef BUS_H
#define BUS_H

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include "CPU.h"
#include "Mapper.h"
#include "PPU.h"

// CPU gets access to memory (including memory-mapped spaces) using the bus.
// See https://bugzmanov.github.io/nes_ebook/chapter_4.html
class Bus {
    static constexpr auto Kb = 1024;

public:
    explicit Bus(std::unique_ptr<Mapper> mapper);

    // CPU and PPU have different buses.

    // cpu read from and write to the bus
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    uint16_t read16(uint16_t addr);
    void write16(uint16_t addr, uint16_t data);

    // ppu read from and write to the bus
    uint8_t ppuRead(uint16_t addr);
    void ppuWrite(uint16_t addr, uint8_t data);

    [[nodiscard]] CPU& getCPU();
    [[nodiscard]] PPU& getPPU();

    [[nodiscard]] const std::array<uint8_t, 2 * Kb>& vRam() const;
    [[nodiscard]] const std::vector<uint8_t>& chrRom() const;

    [[nodiscard]] Mirroring mirroring() const;

private:
    [[nodiscard]] uint16_t mirrorVramAddr(uint16_t addr) const;

    // See https://bugzmanov.github.io/nes_ebook/images/ch2/image_5_motherboard.png
    std::unique_ptr<Mapper> mapper;
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<PPU> ppu;
    std::array<uint8_t, 2 * Kb> cpuRam{};
    std::array<uint8_t, 2 * Kb> ppuRam{};
};

#endif
