#ifndef NES_EMULATOR_BUS_H
#define NES_EMULATOR_BUS_H

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include "CPU.h"
#include "Joypad.h"
#include "Mapper.h"
#include "PPU.h"

// CPU gets access to memory (including memory-mapped spaces) using the bus.
// See https://bugzmanov.github.io/nes_ebook/chapter_4.html
class Bus {
public:
    explicit Bus(std::unique_ptr<Mapper> mapper);

    // CPU and PPU have different buses.

    // CPU read from and write to the MAIN bus
    [[nodiscard]] uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t data);
    [[nodiscard]] uint16_t read16(uint16_t addr) const;
    void write16(uint16_t addr, uint16_t data);

    // PPU read from and write to the PPU bus
    [[nodiscard]] uint8_t ppuRead(uint16_t addr) const;
    void ppuWrite(uint16_t addr, uint8_t data);

    void clock();

    CPU& getCPU();
    PPU& getPPU();
    Joypad& getJoypad();

    [[nodiscard]] const std::vector<uint8_t>& chrRom() const;

private:
    [[nodiscard]] uint16_t mirrorPaletteAddr(uint16_t addr) const;
    [[nodiscard]] uint16_t mirrorVramAddr(uint16_t addr) const;

    // See https://bugzmanov.github.io/nes_ebook/images/ch2/image_5_motherboard.png
    std::unique_ptr<Mapper> mapper;
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<PPU> ppu;
    std::array<uint8_t, 2048> cpuRam{};
    std::array<uint8_t, 2048> ppuRam{};

    Joypad joypad;
};

#endif
